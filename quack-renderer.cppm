export module quack:renderer;
import :thread;
import casein;
import hai;

export namespace quack {
class renderer {
  unsigned m_max_batches;
  hai::uptr<thread> m_thread{};
  unsigned m_ticks{};

  void setup(casein::native_handle_t nptr) {
    m_thread = hai::uptr<thread>::make(nptr, m_max_batches);
    m_thread->start();
  }

  void resize() { m_thread->reset_l1(); }

  void quit() { m_thread = {}; }

public:
  explicit constexpr renderer(unsigned max_batches)
      : m_max_batches{max_batches} {}

  [[nodiscard]] auto allocate_batch(unsigned max_quads) {
    return m_thread->allocate(max_quads);
  }

  [[nodiscard]] constexpr auto ticks() const noexcept { return m_ticks; }

  void process_event(const casein::event &e) {
    switch (e.type()) {
    case casein::CREATE_WINDOW:
      setup(*e.as<casein::events::create_window>());
      break;
    case casein::REPAINT:
      m_ticks++;
      break;
    case casein::RESIZE_WINDOW: {
      resize();
      break;
    }
    case casein::QUIT:
      quit();
      break;
    default:
      break;
    }
  }
};
} // namespace quack
