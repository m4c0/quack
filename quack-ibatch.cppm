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
  vee::pipeline_layout::type m_pl;

  voo::h2l_buffer m_pos;
  voo::h2l_buffer m_colour;
  voo::h2l_buffer m_mult;
  voo::h2l_buffer m_uv;

  dotz::vec2 m_gp{};
  dotz::vec2 m_gs{1, 1};
  unsigned m_count{};

  template <typename Tp>
  static auto create_buf(vee::physical_device pd, unsigned max_quads) {
    auto sz = static_cast<unsigned>(max_quads * sizeof(Tp));
    return voo::h2l_buffer{pd, sz};
  }

public:
  instance_batch() = default;
  instance_batch(vee::physical_device pd, vee::pipeline_layout::type pl,
                 vee::descriptor_set ds, unsigned max_quads)
      : m_pl{pl}, m_pos{create_buf<rect>(pd, max_quads)},
        m_colour{create_buf<colour>(pd, max_quads)},
        m_mult{create_buf<colour>(pd, max_quads)},
        m_uv{create_buf<uv>(pd, max_quads)}, m_count{max_quads} {}

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
    voo::mapmem m{m_colour.host_memory()};
    fn(static_cast<colour *>(*m));
  }
  void map_multipliers(auto &&fn) noexcept {
    voo::mapmem m{m_mult.host_memory()};
    fn(static_cast<colour *>(*m));
  }
  void map_positions(auto &&fn) noexcept {
    voo::mapmem m{m_pos.host_memory()};
    fn(static_cast<rect *>(*m));
  }
  void map_uvs(auto &&fn) noexcept {
    voo::mapmem m{m_uv.host_memory()};
    fn(static_cast<uv *>(*m));
  }
  void map_all(auto &&fn) noexcept {
    struct {
      colour *colours;
      colour *multipliers;
      rect *positions;
      uv *uvs;
    } all;

    voo::mapmem c{m_colour.host_memory()};
    voo::mapmem m{m_mult.host_memory()};
    voo::mapmem p{m_pos.host_memory()};
    voo::mapmem u{m_uv.host_memory()};
    all.colours = static_cast<colour *>(*c);
    all.multipliers = static_cast<colour *>(*m);
    all.positions = static_cast<rect *>(*p);
    all.uvs = static_cast<uv *>(*u);

    fn(all);
  }

  [[nodiscard]] constexpr const auto &count() const noexcept { return m_count; }

  void setup_copy(vee::command_buffer cb) const {
    m_colour.setup_copy(cb);
    m_mult.setup_copy(cb);
    m_pos.setup_copy(cb);
    m_uv.setup_copy(cb);
  }

  void build_commands(vee::command_buffer cb) const {
    if (m_count == 0)
      return;

    upc pc{.grid_pos = m_gp, .grid_size = grid_size()};

    vee::cmd_push_vert_frag_constants(cb, m_pl, &pc);
    vee::cmd_bind_vertex_buffers(cb, 1, m_pos.local_buffer());
    vee::cmd_bind_vertex_buffers(cb, 2, m_colour.local_buffer());
    vee::cmd_bind_vertex_buffers(cb, 3, m_uv.local_buffer());
    vee::cmd_bind_vertex_buffers(cb, 4, m_mult.local_buffer());
  }
};
} // namespace quack
