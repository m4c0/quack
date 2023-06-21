export module quack:ibatch;
import :bbuffer;
import :per_device;
import :objects;
import missingno;
import traits;
import vee;

namespace quack {
class instance_batch {
  vee::pipeline_layout::type m_pl;

  bound_buffer<rect> m_pos;
  bound_buffer<colour> m_colour;
  bound_buffer<colour> m_mult;
  bound_buffer<uv> m_uv;
  upc m_pc;

  pos m_screen_size{};

  unsigned m_count{};

public:
  instance_batch(const per_device *dev, vee::pipeline_layout::type pl,
                 unsigned max_quads)
      : m_pl{pl}, m_pos{bb_vertex{}, dev, max_quads},
        m_colour{bb_vertex{}, dev, max_quads},
        m_uv{bb_vertex{}, dev, max_quads}, m_mult{bb_vertex{}, dev, max_quads},
        m_count{max_quads} {}

  constexpr void resize(unsigned grid_w, unsigned grid_h, float sw,
                        float sh) noexcept {
    float aspect = sw / sh;
    float gw = grid_w / 2.0;
    float gh = grid_h / 2.0;
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
    m_pos.map([&](auto *is) {
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
    });
    return res;
  }

  [[nodiscard]] constexpr const auto &multipliers() const noexcept {
    return m_mult;
  }
  [[nodiscard]] constexpr const auto &colours() const noexcept {
    return m_colour;
  }
  [[nodiscard]] constexpr const auto &positions() const noexcept {
    return m_pos;
  }
  [[nodiscard]] constexpr const auto &uvs() const noexcept { return m_uv; }

  [[nodiscard]] constexpr const auto &count() const noexcept { return m_count; }
  [[nodiscard]] constexpr const auto &push_constants() const noexcept {
    return m_pc;
  }

  void build_commands(vee::command_buffer cb) const {
    vee::cmd_push_vert_frag_constants(cb, m_pl, &m_pc);
    vee::cmd_bind_vertex_buffers(cb, 1, *m_pos);
    vee::cmd_bind_vertex_buffers(cb, 2, *m_colour);
    vee::cmd_bind_vertex_buffers(cb, 3, *m_uv);
    vee::cmd_bind_vertex_buffers(cb, 4, *m_mult);
    vee::cmd_draw(cb, v_count, m_count);
  }
};
} // namespace quack
