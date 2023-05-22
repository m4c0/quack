export module quack:ilayout;
import :ibatch;
import :objects;
import :renderer;
import casein;

namespace quack {
export template <typename Tp, unsigned N> class instance_layout;

export template <unsigned N> class instance_layout<void, N> {
  renderer *m_r;
  instance_batch *m_batch;
  pos m_mouse_pos{};

  void mouse_move(float x, float y) { m_mouse_pos = {x, y}; }

protected:
  virtual void setup() { m_batch = m_r->allocate_batch(N); }
  virtual void resize(unsigned w, unsigned h) {}

public:
  explicit constexpr instance_layout(renderer *r) : m_r{r} {}

  [[nodiscard]] constexpr auto *batch() noexcept { return m_batch; }

  [[nodiscard]] constexpr auto current_hover() noexcept {
    return m_batch->current_hover(m_mouse_pos);
  }

  void process_event(const casein::event &e) {
    switch (e.type()) {
    case casein::CREATE_WINDOW:
      setup();
      break;
    case casein::MOUSE_DOWN:
    case casein::MOUSE_MOVE:
    case casein::MOUSE_UP: {
      const auto &[x, y] = *e.as<casein::events::mouse_move>();
      mouse_move(x, y);
      break;
    }
    case casein::RESIZE_WINDOW: {
      const auto &[w, h, s, l] = *e.as<casein::events::resize_window>();
      resize(w, h);
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
};

export template <typename Tp, unsigned N>
class instance_layout : public instance_layout<void, N> {
  Tp m_data[N];

public:
  using instance_layout<void, N>::instance_layout;

  void fill_colour(auto &&fn) {
    this->batch()->colours().map([&](auto *c) {
      for (auto i = 0; i < N; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_pos(auto &&fn) {
    this->batch()->positions().map([&](auto *c) {
      for (auto i = 0; i < N; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_mult(auto &&fn) {
    this->batch()->multipliers().map([&](auto *c) {
      for (auto i = 0; i < N; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_uv(auto &&fn) {
    this->batch()->uvs().map([&](auto *c) {
      for (auto i = 0; i < N; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void reset_grid() {
    for (auto &d : m_data)
      d = {};
  }

  [[nodiscard]] constexpr auto &data() noexcept { return m_data; }
  [[nodiscard]] constexpr auto &at(unsigned idx) noexcept {
    return m_data[idx];
  }
  [[nodiscard]] constexpr const auto &at(unsigned idx) const noexcept {
    return m_data[idx];
  }
};
} // namespace quack
