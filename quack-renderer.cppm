export module quack:renderer;
import :agg;
import :raii;
import :objects;
import :thread;
import casein;
import hai;
import missingno;
import traits;
import vee;

export namespace quack {
class renderer {
  unsigned m_max_batches;
  hai::uptr<thread> m_thread{};

  void setup(casein::native_handle_t nptr) {
    m_thread = hai::uptr<thread>::make(nptr, m_max_batches);
    m_thread->start();
  }

  void resize() { m_thread->reset_l1(); }

  void quit() { m_thread = {}; }

public:
  explicit constexpr renderer(unsigned max_batches)
      : m_max_batches{max_batches} {}

  void load_atlas(unsigned w, unsigned h, auto &&fn) {
    m_thread->load_atlas(w, h, traits::fwd<decltype(fn)>(fn));
  }

  [[nodiscard]] auto allocate_batch(unsigned max_quads) {
    return m_thread->allocate(max_quads);
  }

  void process_event(const casein::event &e) {
    switch (e.type()) {
    case casein::CREATE_WINDOW:
      setup(*e.as<casein::events::create_window>());
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
