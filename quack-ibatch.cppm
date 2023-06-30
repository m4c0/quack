export module quack:ibatch;
import :bbuffer;
import :per_device;
import :objects;
import :stage;
import missingno;
import traits;
import vee;

namespace quack {
class instance_batch {
  vee::pipeline_layout::type m_pl;
  vee::descriptor_set m_desc_set;
  vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);

  stage_image m_atlas;
  bound_buffer<rect> m_pos;
  bound_buffer<colour> m_colour;
  bound_buffer<colour> m_mult;
  bound_buffer<uv> m_uv;
  upc m_pc;

  pos m_screen_size{};

  unsigned m_gw{1};
  unsigned m_gh{1};
  unsigned m_count{};

public:
  instance_batch(const per_device *dev, vee::pipeline_layout::type pl,
                 vee::descriptor_set ds, unsigned max_quads)
      : m_pl{pl}, m_desc_set{ds}, m_atlas{dev},
        m_pos{bb_vertex{}, dev, max_quads},
        m_colour{bb_vertex{}, dev, max_quads},
        m_uv{bb_vertex{}, dev, max_quads}, m_mult{bb_vertex{}, dev, max_quads},
        m_count{max_quads} {}

  constexpr void set_grid(unsigned gw, unsigned gh) noexcept {
    m_gw = gw;
    m_gh = gh;
  }

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
  [[nodiscard]] mno::opt<unsigned> current_hover(pos mouse_pos) {
    auto [mx, my] = translate_mouse_pos(mouse_pos);

    mno::opt<unsigned> res{};
    auto m = m_pos.map();
    auto *is = static_cast<rect *>(*m);
    for (auto i = 0U; i < m_count; i++) {
      if (mx < is[i].x)
        continue;
      if (my < is[i].y)
        continue;
      if (mx > is[i].x + is[i].w)
        continue;
      if (my > is[i].y + is[i].h)
        continue;
      res = i;
      break;
    }
    return res;
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

  [[nodiscard]] constexpr const auto &count() const noexcept { return m_count; }
  [[nodiscard]] constexpr const auto &push_constants() const noexcept {
    return m_pc;
  }

  void load_atlas(unsigned w, unsigned h, auto &&fn) {
    if (m_atlas.resize_image(w, h)) {
      const auto &iv = m_atlas.image_view();
      vee::update_descriptor_set(m_desc_set, 0, *iv, *m_smp);
    }

    m_atlas.load_image(traits::fwd<decltype(fn)>(fn));
  }

  void build_commands(vee::command_buffer cb) const {
    if (m_count == 0)
      return;

    vee::cmd_push_vert_frag_constants(cb, m_pl, &m_pc);
    vee::cmd_bind_vertex_buffers(cb, 1, *m_pos);
    vee::cmd_bind_vertex_buffers(cb, 2, *m_colour);
    vee::cmd_bind_vertex_buffers(cb, 3, *m_uv);
    vee::cmd_bind_vertex_buffers(cb, 4, *m_mult);
    vee::cmd_bind_descriptor_set(cb, m_pl, 0, m_desc_set);
    vee::cmd_draw(cb, v_count, m_count);
  }
  void build_atlas_commands(vee::command_buffer cb) {
    m_atlas.build_commands(cb);
  }
};
} // namespace quack
