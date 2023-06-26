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
  unsigned m_max_instances;

public:
  explicit constexpr ilayout(renderer *r, unsigned n)
      : m_r{r}, m_max_instances{n} {}

  [[nodiscard]] constexpr auto &operator*() noexcept { return *m_batch; }
  [[nodiscard]] constexpr const auto &operator*() const noexcept {
    return *m_batch;
  }
  [[nodiscard]] constexpr auto *operator->() noexcept { return m_batch; }
  [[nodiscard]] constexpr const auto *operator->() const noexcept {
    return m_batch;
  }

  void process_event(const casein::event &e) {
    switch (e.type()) {
    case casein::CREATE_WINDOW:
      m_batch = m_r->allocate_batch(m_max_instances);
      break;
    case casein::RESIZE_WINDOW: {
      const auto &[w, h, s, l] = *e.as<casein::events::resize_window>();
      m_batch->resize(w, h);
      break;
    }
    default:
      break;
    }
  }
};
} // namespace quack
