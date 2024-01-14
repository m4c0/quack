export module quack:ibatch;
import :extent;
import :mouse;
import :per_device;
import :objects;
import dotz;
import silog;
import traits;
import vee;
import voo;

namespace quack {
class instance_batch {
  const per_device *m_dev;
  vee::pipeline_layout::type m_pl;
  vee::descriptor_set m_desc_set;
  vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);

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
  static auto create_buf(const per_device *dev, unsigned max_quads) {
    return voo::h2l_buffer{dev->physical_device(), dev->command_pool(),
                           static_cast<unsigned>(max_quads * sizeof(Tp))};
  }

public:
  instance_batch(const per_device *dev, vee::pipeline_layout::type pl,
                 vee::descriptor_set ds, unsigned max_quads)
      : m_dev{dev}, m_pl{pl}, m_desc_set{ds},
        m_pos{create_buf<rect>(dev, max_quads)},
        m_colour{create_buf<colour>(dev, max_quads)},
        m_mult{create_buf<colour>(dev, max_quads)},
        m_uv{create_buf<uv>(dev, max_quads)}, m_count{max_quads} {}

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
    auto m = m_colour.mapmem();
    fn(static_cast<colour *>(*m));
  }
  void map_multipliers(auto &&fn) noexcept {
    auto m = m_mult.mapmem();
    fn(static_cast<colour *>(*m));
  }
  void map_positions(auto &&fn) noexcept {
    auto m = m_pos.mapmem();
    fn(static_cast<rect *>(*m));
  }
  void map_uvs(auto &&fn) noexcept {
    auto m = m_uv.mapmem();
    fn(static_cast<uv *>(*m));
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
    all.colours = static_cast<colour *>(*c);
    all.multipliers = static_cast<colour *>(*m);
    all.positions = static_cast<rect *>(*p);
    all.uvs = static_cast<uv *>(*u);

    fn(all);
  }

  [[nodiscard]] constexpr const auto &count() const noexcept { return m_count; }

  void load_atlas(unsigned w, unsigned h, auto &&fn) {
    auto a =
        voo::h2l_image(m_dev->physical_device(), m_dev->command_pool(), w, h);
    {
      auto m = a.mapmem();
      fn(static_cast<u8_rgba *>(*m));
    }

    load_atlas(traits::move(a));
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
  void build_atlas_commands(vee::queue q) {
    m_atlas.submit(q);
    m_pos.submit(q);
    m_colour.submit(q);
    m_uv.submit(q);
    m_mult.submit(q);
  }
};
} // namespace quack
