export module quack:mouse;
import :extent;
import casein;
import dotz;

namespace quack {
export class mouse_tracker {
  dotz::vec2 m_mouse_pos{};

  void mouse_move(float x, float y) { m_mouse_pos = {x, y}; }

public:
  [[nodiscard]] constexpr auto mouse_pos() const noexcept {
    return m_mouse_pos;
  }
  [[nodiscard]] auto mouse_pos_rel() const noexcept {
    return m_mouse_pos / extent_tracker::instance().screen_size();
  }

  void handle(const casein::event &e) {
    extent_tracker::instance().handle(e);

    switch (e.type()) {
    case casein::MOUSE_DOWN:
    case casein::MOUSE_MOVE:
    case casein::MOUSE_UP: {
      const auto &[x, y] = *e.as<casein::events::mouse_move>();
      mouse_move(x, y);
      break;
    }
    case casein::TOUCH_DOWN:
    case casein::TOUCH_MOVE:
    case casein::TOUCH_UP: {
      const auto &[x, y, lp] = *e.as<casein::events::touch_down>();
      mouse_move(x, y);
      break;
    }
    default:
      break;
    }
  }

  static auto &instance() {
    static mouse_tracker i{};
    return i;
  }
};
} // namespace quack
