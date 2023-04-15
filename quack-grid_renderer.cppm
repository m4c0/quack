export module quack:grid_renderer;
import :objects;
import :renderer;
import casein;

namespace quack {
export template <auto W, auto H, typename Tp> class grid_renderer {
  renderer m_r;
  Tp m_data[W * H];

public:
  static constexpr const auto width = W;
  static constexpr const auto height = H;
  static constexpr const auto cells = width * height;

  grid_renderer()
      : m_r(params{
            .grid_w = W,
            .grid_h = H,
            .max_quads = W * H,
        }) {}

  void setup(casein::native_handle_t nptr) {
    m_r.setup(nptr);
    m_r.fill_pos([](pos *is) {
      unsigned i = 0;
      for (auto y = 0; y < H; y++) {
        for (auto x = 0; x < W; x++, i++) {
          is[i].x = x;
          is[i].y = y;
        }
      }
    });
  }
  void load_atlas(unsigned w, unsigned h, auto &&fn) {
    m_r.load_atlas(w, h, fn);
  }
  void fill_colour(auto &&fn) {
    m_r.fill_colour([&](auto *c) {
      for (auto i = 0; i < cells; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_uv(auto &&fn) {
    m_r.fill_uv([&](auto *c) {
      for (auto i = 0; i < cells; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void reset_grid() {
    for (auto &d : m_data)
      d = {};
  }
  void resize(unsigned w, unsigned h, float scale) { m_r.resize(w, h, scale); }
  void repaint() { m_r.set_icount(cells); }
  void quit() { m_r.quit(); }

  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) noexcept {
    return m_data[y * W + x];
  }
  [[nodiscard]] constexpr auto &at(unsigned idx) noexcept {
    return m_data[idx];
  }

  [[nodiscard]] constexpr auto current_hover() noexcept {
    return m_r.current_hover();
  }

  void process_event(const casein::event &e) {
    switch (e.type()) {
    case casein::CREATE_WINDOW:
      setup(*e.as<casein::events::create_window>());
      break;
    case casein::REPAINT:
      repaint();
      break;
    default:
      m_r.process_event(e);
      break;
    }
  }
};
} // namespace quack
