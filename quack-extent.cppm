export module quack:extent;
import casein;
import dotz;

namespace quack {
export class extent_tracker {
  dotz::vec2 m_screen_size{};

public:
  [[nodiscard]] constexpr auto screen_size() const noexcept {
    return m_screen_size;
  }
  [[nodiscard]] constexpr auto screen_aspect() const noexcept {
    return m_screen_size.x / m_screen_size.y;
  }

  void handle(const casein::event &e) {
    switch (e.type()) {
    case casein::RESIZE_WINDOW: {
      const auto &[w, h, _, __] = *e.as<casein::events::resize_window>();
      m_screen_size = {w, h};
      break;
    }
    default:
      break;
    }
  }
  static auto &instance() {
    static extent_tracker i{};
    return i;
  }
};
} // namespace quack
