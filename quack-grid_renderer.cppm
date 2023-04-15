export module quack:grid_renderer;
import :ibatch;
import :objects;
import :renderer;
import casein;

namespace quack {
export template <auto W, auto H, typename Tp> class grid_renderer {
  renderer m_r{};
  instance_batch *m_batch;
  Tp m_data[W * H];
  pos m_mouse_pos{};

  void mouse_move(float x, float y) { m_mouse_pos = {x, y}; }

  void setup() {
    m_batch = m_r.allocate_batch(cells);
    m_batch->set_count(cells);
    m_batch->positions().map([](pos *is) {
      unsigned i = 0;
      for (auto y = 0; y < H; y++) {
        for (auto x = 0; x < W; x++, i++) {
          is[i].x = x;
          is[i].y = y;
        }
      }
    });
  }
  void resize(unsigned w, unsigned h) { m_batch->resize(W, H, w, h); }

public:
  static constexpr const auto width = W;
  static constexpr const auto height = H;
  static constexpr const auto cells = width * height;

  void load_atlas(unsigned w, unsigned h, auto &&fn) {
    m_r.load_atlas(w, h, fn);
  }
  void fill_colour(auto &&fn) {
    m_batch->colours().map([&](auto *c) {
      for (auto i = 0; i < cells; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_uv(auto &&fn) {
    m_batch->uvs().map([&](auto *c) {
      for (auto i = 0; i < cells; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void reset_grid() {
    for (auto &d : m_data)
      d = {};
  }

  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) noexcept {
    return m_data[y * W + x];
  }
  [[nodiscard]] constexpr auto &at(unsigned idx) noexcept {
    return m_data[idx];
  }

  [[nodiscard]] constexpr auto current_hover() noexcept {
    return m_batch->current_hover(m_mouse_pos);
  }

  void process_event(const casein::event &e) {
    m_r.process_event(e);

    switch (e.type()) {
    case casein::CREATE_WINDOW:
      setup();
      break;
    case casein::MOUSE_DOWN: {
      const auto &[x, y, btn] = *e.as<casein::events::mouse_down>();
      mouse_move(x, y);
      break;
    }
    case casein::MOUSE_UP: {
      const auto &[x, y, btn] = *e.as<casein::events::mouse_up>();
      mouse_move(x, y);
      break;
    }
    case casein::MOUSE_MOVE: {
      const auto &[x, y] = *e.as<casein::events::mouse_move>();
      mouse_move(x, y);
      break;
    }
    case casein::RESIZE_WINDOW: {
      const auto &[w, h, s, l] = *e.as<casein::events::resize_window>();
      resize(w, h);
      break;
    }
    default:
      break;
    }
  }
};
} // namespace quack
