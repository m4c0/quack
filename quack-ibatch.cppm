export module quack:ibatch;
import :bbuffer;
import :per_device;
import :objects;
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
  bound_buffer<rect> m_pos;
  bound_buffer<colour> m_colour;
  bound_buffer<colour> m_mult;
  bound_buffer<uv> m_uv;
  upc m_pc;

  pos m_screen_size{};

  unsigned m_gw{1};
  unsigned m_gh{1};
  unsigned m_count{};
  bool m_dset_loaded{};

public:
  instance_batch(const per_device *dev, vee::pipeline_layout::type pl,
                 vee::descriptor_set ds, unsigned max_quads)
      : m_dev{dev}, m_pl{pl}, m_desc_set{ds},
        m_pos{bb_vertex{}, dev, max_quads},
        m_colour{bb_vertex{}, dev, max_quads},
        m_mult{bb_vertex{}, dev, max_quads}, m_uv{bb_vertex{}, dev, max_quads},
        m_count{max_quads} {}

  constexpr void set_grid(unsigned gw, unsigned gh) noexcept {
    m_gw = gw;
    m_gh = gh;
  }
  constexpr auto grid_size() const noexcept { return m_pc.grid_size; }

  constexpr void resize(float sw, float sh) noexcept {
    float aspect = sw / sh;
    float gw = m_gw / 2.0;
    float gh = m_gh / 2.0;
    float grid_aspect = gw / gh;
    m_pc.grid_size =
        grid_aspect < aspect ? size{aspect * gh, gh} : size{gw, gw / aspect};
    m_screen_size = {sw, sh};
  }

  constexpr void center_at(float gx, float gy) { m_pc.grid_pos = pos{gx, gy}; }
  constexpr auto center() const noexcept { return m_pc.grid_pos; }

  constexpr void set_count(unsigned c) noexcept { m_count = c; }

  [[nodiscard]] pos translate_mouse_pos(pos mouse_pos) const noexcept {
    pos screen_scale = {2.0f * m_pc.grid_size.w / m_screen_size.x,
                        2.0f * m_pc.grid_size.h / m_screen_size.y};
    pos screen_disp = {m_pc.grid_size.w - m_pc.grid_pos.x,
                       m_pc.grid_size.h - m_pc.grid_pos.y};

    auto mx = mouse_pos.x * screen_scale.x - screen_disp.x;
    auto my = mouse_pos.y * screen_scale.y - screen_disp.y;
    return {mx, my};
  }

  void map_colours(auto &&fn) const noexcept {
    auto m = m_colour.map();
    fn(static_cast<colour *>(*m));
  }
  void map_multipliers(auto &&fn) const noexcept {
    auto m = m_mult.map();
    fn(static_cast<colour *>(*m));
  }
  void map_positions(auto &&fn) const noexcept {
    auto m = m_pos.map();
    fn(static_cast<rect *>(*m));
  }
  void map_uvs(auto &&fn) const noexcept {
    auto m = m_uv.map();
    fn(static_cast<uv *>(*m));
  }
  void map_all(auto &&fn) const noexcept {
    struct {
      colour *colours;
      colour *multipliers;
      rect *positions;
      uv *uvs;
    } all;

    auto c = m_colour.map();
    auto m = m_mult.map();
    auto p = m_pos.map();
    auto u = m_uv.map();
    all.colours = static_cast<colour *>(*c);
    all.multipliers = static_cast<colour *>(*m);
    all.positions = static_cast<rect *>(*p);
    all.uvs = static_cast<uv *>(*u);

    fn(all);
  }

  [[nodiscard]] constexpr const auto &count() const noexcept { return m_count; }
  [[nodiscard]] constexpr const auto &push_constants() const noexcept {
    return m_pc;
  }

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

  void build_commands(vee::command_buffer cb) const {
    if (m_count == 0)
      return;

    vee::cmd_push_vert_frag_constants(cb, m_pl, &m_pc);
    vee::cmd_bind_vertex_buffers(cb, 1, *m_pos);
    vee::cmd_bind_vertex_buffers(cb, 2, *m_colour);
    vee::cmd_bind_vertex_buffers(cb, 3, *m_uv);
    vee::cmd_bind_vertex_buffers(cb, 4, *m_mult);
    if (m_dset_loaded)
      vee::cmd_bind_descriptor_set(cb, m_pl, 0, m_desc_set);
    vee::cmd_draw(cb, v_count, m_count);
  }
  void build_atlas_commands(vee::queue q) { m_atlas.submit(q); }
};
} // namespace quack
