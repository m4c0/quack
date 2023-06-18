export module quack:ilayout;
import :ibatch;
import :mouse;
import :objects;
import :renderer;
import casein;

namespace quack {
export class ilayout {
  renderer *m_r;
  instance_batch *m_batch;
  unsigned m_gw;
  unsigned m_gh;
  unsigned m_max_instances;

public:
  explicit constexpr ilayout(renderer *r, unsigned n)
      : m_r{r}, m_max_instances{n} {}

  [[nodiscard]] constexpr auto *batch() noexcept { return m_batch; }

  constexpr void set_grid(unsigned gw, unsigned gh) noexcept {
    m_gw = gw;
    m_gh = gh;
  }

  void process_event(const casein::event &e) {
    switch (e.type()) {
    case casein::CREATE_WINDOW:
      m_batch = m_r->allocate_batch(m_max_instances);
      break;
    case casein::RESIZE_WINDOW: {
      const auto &[w, h, s, l] = *e.as<casein::events::resize_window>();
      m_batch->resize(m_gw, m_gh, w, h);
      break;
    }
    default:
      break;
    }
  }
};

export template <typename Tp, unsigned N> class instance_layout {
  ilayout m_il;
  Tp m_data[N];

public:
  explicit constexpr instance_layout(renderer *r) : m_il{r, N} {}

  [[nodiscard]] constexpr auto *batch() noexcept { return m_il.batch(); }
  void process_event(const casein::event &e) { m_il.process_event(e); }

  constexpr void set_grid(unsigned gw, unsigned gh) noexcept {
    m_il.set_grid(gw, gh);
  }

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
