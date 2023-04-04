export module quack:renderer;
import :objects;
import casein;
import hai;
import missingno;

export namespace quack {
class pimpl {
public:
  pimpl() {}
  virtual ~pimpl() {}

  virtual void fill_colour(const filler<colour> &) = 0;
  virtual void fill_pos(const filler<pos> &) = 0;
  virtual void fill_uv(const filler<uv> &) = 0;
  virtual void load_atlas(unsigned w, unsigned h, const filler<u8_rgba> &) = 0;

  virtual void setup(casein::native_handle_t nptr) = 0;
  virtual void repaint(unsigned i_count) = 0;
  virtual void resize(unsigned w, unsigned h, float scale) = 0;

  [[nodiscard]] virtual mno::opt<unsigned> current_hover() = 0;
  virtual void mouse_move(unsigned x, unsigned y) = 0;
};
class renderer {
  hai::uptr<pimpl> m_pimpl;

  template <typename Tp, typename Fn> class s : public filler<Tp> {
    Fn m;

  public:
    constexpr s(Fn &&fn) : m{fn} {}
    void operator()(Tp *c) const noexcept { m(c); }
  };

public:
  renderer(const params &p);
  ~renderer() = default;

  void setup(casein::native_handle_t nptr) { m_pimpl->setup(nptr); }
  void repaint(unsigned i_count) { m_pimpl->repaint(i_count); }
  void resize(unsigned w, unsigned h, float scale) {
    m_pimpl->resize(w, h, scale);
  }
  void quit() { m_pimpl = {}; }

  template <typename Fn> void fill_pos(Fn &&fn) {
    m_pimpl->fill_pos(s<pos, Fn>{static_cast<Fn &&>(fn)});
  }
  template <typename Fn> void fill_colour(Fn &&fn) {
    m_pimpl->fill_colour(s<colour, Fn>{static_cast<Fn &&>(fn)});
  }
  template <typename Fn> void fill_uv(Fn &&fn) {
    m_pimpl->fill_uv(s<uv, Fn>{static_cast<Fn &&>(fn)});
  }
  template <typename Fn> void load_atlas(unsigned w, unsigned h, Fn &&fn) {
    m_pimpl->load_atlas(w, h, s<u8_rgba, Fn>{static_cast<Fn &&>(fn)});
  }

  [[nodiscard]] mno::opt<unsigned> current_hover() {
    return m_pimpl->current_hover();
  }

  void mouse_move(unsigned w, unsigned h) { m_pimpl->mouse_move(w, h); }

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
