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
  hai::uptr<thread> m_thread{};
  instance_batch *m_batch{};
  params m_p;
  pos m_mouse_pos{};

public:
  explicit renderer(const params &p) : m_p{p} {}

  void setup(casein::native_handle_t nptr) {
    m_thread = hai::uptr<thread>::make(nptr, 1U);
    m_batch = m_thread->allocate(m_p.max_quads);
    m_thread->start();
  }

  void resize(unsigned w, unsigned h, float scale) {
    m_batch->resize(m_p, w, h);
    m_thread->reset_l1();
  }

  void set_icount(unsigned i_count) { m_batch->set_count(i_count); }

  void quit() { m_thread = {}; }

  template <typename Fn> void fill_pos(Fn &&fn) {
    m_batch->positions().map(traits::move(fn));
  }
  template <typename Fn> void fill_colour(Fn &&fn) {
    m_batch->colours().map(traits::move(fn));
  }
  template <typename Fn> void fill_uv(Fn &&fn) {
    m_batch->uvs().map(traits::move(fn));
  }
  template <typename Fn> void load_atlas(unsigned w, unsigned h, Fn &&fn) {
    m_thread->load_atlas(w, h, traits::move(fn));
  }

  void mouse_move(float x, float y) { m_mouse_pos = {x, y}; }
  [[nodiscard]] mno::opt<unsigned> current_hover() {
    return m_batch->current_hover(m_mouse_pos);
  }

  void process_event(const casein::event &e) {
    switch (e.type()) {
    case casein::CREATE_WINDOW:
      setup(*e.as<casein::events::create_window>());
      break;
    case casein::RESIZE_WINDOW: {
      const auto &[w, h, scale, live] = *e.as<casein::events::resize_window>();
      resize(w, h, scale);
      break;
    }
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
    case casein::QUIT:
      quit();
      break;
    default:
      break;
    }
  }
};
} // namespace quack
