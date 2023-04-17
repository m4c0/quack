export module quack:grid_renderer;
import :ibatch;
import :objects;
import :renderer;
import casein;

namespace quack {
export template <typename Tp, unsigned N> class instance_layout {
  renderer *m_r;
  instance_batch *m_batch;
  Tp m_data[N];
  pos m_mouse_pos{};

  void mouse_move(float x, float y) { m_mouse_pos = {x, y}; }

protected:
  virtual void setup() { m_batch = m_r->allocate_batch(N); }
  virtual void resize(unsigned w, unsigned h) {}

public:
  explicit constexpr instance_layout(renderer *r) : m_r{r} {}

  [[nodiscard]] constexpr auto *batch() noexcept { return m_batch; }

  void fill_colour(auto &&fn) {
    m_batch->colours().map([&](auto *c) {
      for (auto i = 0; i < N; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_pos(auto &&fn) {
    m_batch->positions().map([&](auto *c) {
      for (auto i = 0; i < N; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_uv(auto &&fn) {
    m_batch->uvs().map([&](auto *c) {
      for (auto i = 0; i < N; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void reset_grid() {
    for (auto &d : m_data)
      d = {};
  }

  [[nodiscard]] constexpr auto &at(unsigned idx) noexcept {
    return m_data[idx];
  }

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

export template <auto W, auto H, typename Tp>
class grid_ilayout : public instance_layout<Tp, W * H> {
  using parent_t = instance_layout<Tp, W * H>;

  void setup() override {
    parent_t::setup();

    this->batch()->set_count(cells);
    this->batch()->positions().map([](pos *is) {
      unsigned i = 0;
      for (auto y = 0; y < H; y++) {
        for (auto x = 0; x < W; x++, i++) {
          is[i].x = x;
          is[i].y = y;
        }
      }
    });
  };
  void resize(unsigned w, unsigned h) override {
    this->batch()->resize(W, H, w, h);
  }

public:
  static constexpr const auto width = W;
  static constexpr const auto height = H;
  static constexpr const auto cells = width * height;

  using parent_t::at;
  using parent_t::parent_t;

  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) noexcept {
    return parent_t::at(y * W + x);
  }
};
} // namespace quack
