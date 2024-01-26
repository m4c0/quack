export module quack:ibatch;
import :extent;
import :mouse;
import :objects;
import dotz;
import jute;
import silog;
import traits;
import vee;
import voo;

namespace quack {
export class instance_batch {
  vee::physical_device m_pd;
  vee::command_pool::type m_cp;

  vee::pipeline_layout::type m_pl;
  vee::descriptor_set m_desc_set;
  vee::sampler m_smp;

  voo::h2l_image m_atlas{};
  voo::h2l_buffer m_pos;
  voo::h2l_buffer m_colour;
  voo::h2l_buffer m_mult;
  voo::h2l_buffer m_uv;

  dotz::vec2 m_gp{};
  dotz::vec2 m_gs{1, 1};
  unsigned m_count{};
  bool m_dset_loaded{};

  template <typename Tp>
  static auto create_buf(vee::physical_device pd, vee::command_pool::type cp,
                         unsigned max_quads) {
    auto sz = static_cast<unsigned>(max_quads * sizeof(Tp));
    return voo::h2l_buffer{pd, cp, sz};
  }

public:
  instance_batch() = default;
  instance_batch(vee::physical_device pd, vee::command_pool::type cp,
                 vee::pipeline_layout::type pl, vee::descriptor_set ds,
                 unsigned max_quads)
      : m_pd{pd}, m_cp{cp}, m_pl{pl}, m_desc_set{ds},
        m_smp{vee::create_sampler(vee::nearest_sampler)},
        m_pos{create_buf<rect>(pd, cp, max_quads)},
        m_colour{create_buf<colour>(pd, cp, max_quads)},
        m_mult{create_buf<colour>(pd, cp, max_quads)},
        m_uv{create_buf<uv>(pd, cp, max_quads)}, m_count{max_quads} {}

  constexpr void set_grid(unsigned gw, unsigned gh) noexcept {
    m_gs = dotz::vec2{gw, gh};
  }
  auto grid_size() const noexcept {
    float aspect = extent_tracker::instance().screen_aspect();
    float gw = m_gs.x / 2.0;
    float gh = m_gs.y / 2.0;
    float grid_aspect = gw / gh;
    return grid_aspect < aspect ? dotz::vec2{aspect * gh, gh}
                                : dotz::vec2{gw, gw / aspect};
  }

  constexpr void center_at(float gx, float gy) { m_gp = {gx, gy}; }
  constexpr auto center() const noexcept { return m_gp; }

  constexpr void set_count(unsigned c) noexcept { m_count = c; }

  [[nodiscard]] auto mouse_pos() const noexcept {
    auto screen_scale = m_gs * 2.0f / extent_tracker::instance().screen_size();
    auto screen_disp = m_gs - m_gp;
    auto mouse_pos = mouse_tracker::instance().mouse_pos();
    return mouse_pos * screen_scale / screen_disp;
  }

  void map_colours(auto &&fn) noexcept {
    m_colour.mapmem()
        .map([&](auto &&m) { fn(static_cast<colour *>(*m)); })
        .take([](auto msg) {
          silog::log(silog::warning, "Failed to load colours: %s", msg);
        });
  }
  void map_multipliers(auto &&fn) noexcept {
    m_mult.mapmem()
        .map([&](auto &&m) { fn(static_cast<colour *>(*m)); })
        .take([](auto msg) {
          silog::log(silog::warning, "Failed to load multipliers: %s", msg);
        });
  }
  void map_positions(auto &&fn) noexcept {
    m_pos.mapmem()
        .map([&](auto &&m) { fn(static_cast<rect *>(*m)); })
        .take([](auto msg) {
          silog::log(silog::warning, "Failed to load positions: %s", msg);
        });
  }
  void map_uvs(auto &&fn) noexcept {
    m_uv.mapmem()
        .map([&](auto &&m) { fn(static_cast<uv *>(*m)); })
        .take([](auto msg) {
          silog::log(silog::warning, "Failed to load uvs: %s", msg);
        });
  }
  void map_all(auto &&fn) noexcept {
    struct {
      colour *colours;
      colour *multipliers;
      rect *positions;
      uv *uvs;
    } all;

    auto c = m_colour.mapmem();
    auto m = m_mult.mapmem();
    auto p = m_pos.mapmem();
    auto u = m_uv.mapmem();
    all.colours = c.map([](auto &c) {
                     return static_cast<colour *>(*c);
                   }).unwrap(nullptr);
    all.multipliers = m.map([](auto &m) {
                         return static_cast<colour *>(*m);
                       }).unwrap(nullptr);
    all.positions =
        p.map([](auto &p) { return static_cast<rect *>(*p); }).unwrap(nullptr);
    all.uvs =
        u.map([](auto &u) { return static_cast<uv *>(*u); }).unwrap(nullptr);

    fn(all);
  }

  [[nodiscard]] constexpr const auto &count() const noexcept { return m_count; }
  [[nodiscard]] constexpr auto atlas_size() const noexcept {
    return dotz::vec2{m_atlas.width(), m_atlas.height()};
  }

  void load_atlas(unsigned w, unsigned h, auto &&fn) {
    auto a = voo::h2l_image(m_pd, m_cp, w, h);
    a.mapmem()
        .map([&](auto &&m) { fn(static_cast<u8_rgba *>(*m)); })
        .take([](auto err) {
          silog::log(silog::warning, "Failed to load atlas: %s", err);
        });

    load_atlas(traits::move(a));
  }
  void load_atlas(jute::view file) {
    voo::load_sires_image(file, m_pd, m_cp)
        .map([this](auto &&img) { load_atlas(traits::move(img)); })
        .take([](auto err) {
          silog::log(silog::warning, "Failed to load atlas: %s", err);
        });
  }
  void load_atlas(voo::h2l_image &&img) {
    m_atlas = traits::move(img);

    vee::update_descriptor_set(m_desc_set, 0, m_atlas.iv(), *m_smp);
    m_dset_loaded = true;
  }

  [[nodiscard]] int build_commands(vee::command_buffer cb) const {
    if (m_count == 0)
      return 0;

    upc pc{.grid_pos = m_gp, .grid_size = grid_size()};

    vee::cmd_push_vert_frag_constants(cb, m_pl, &pc);
    vee::cmd_bind_vertex_buffers(cb, 1, m_pos.buffer());
    vee::cmd_bind_vertex_buffers(cb, 2, m_colour.buffer());
    vee::cmd_bind_vertex_buffers(cb, 3, m_uv.buffer());
    vee::cmd_bind_vertex_buffers(cb, 4, m_mult.buffer());
    if (m_dset_loaded)
      vee::cmd_bind_descriptor_set(cb, m_pl, 0, m_desc_set);

    return m_count;
  }
  void submit_buffers(vee::queue q) {
    m_atlas.submit(q);
    m_pos.submit(q);
    m_colour.submit(q);
    m_uv.submit(q);
    m_mult.submit(q);
  }
  void submit_buffers(const voo::device_and_queue &dq) {
    submit_buffers(dq.queue());
  }
};
} // namespace quack
