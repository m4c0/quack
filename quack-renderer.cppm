export module quack:renderer;
import :objects;
import casein;
import hai;

export namespace quack {
class pimpl;
class renderer {
  hai::uptr<pimpl> m_pimpl;

  void _fill_colour(const filler<colour> &);
  void _fill_pos(const filler<pos> &);
  void _fill_uv(const filler<uv> &);
  void _load_atlas(unsigned w, unsigned h, const filler<u8_rgba> &);

  template <typename Tp, typename Fn> class s : public filler<Tp> {
    Fn m;

  public:
    constexpr s(Fn &&fn) : m{fn} {}
    void operator()(Tp *c) const noexcept { m(c); }
  };

public:
  renderer(const params &p);
  ~renderer();

  void setup(casein::native_handle_t);
  void repaint(unsigned i_count);
  void resize(unsigned w, unsigned h);
  void quit();

  template <typename Fn> void fill_pos(Fn &&fn) {
    _fill_pos(s<pos, Fn>{static_cast<Fn &&>(fn)});
  }
  template <typename Fn> void fill_colour(Fn &&fn) {
    _fill_colour(s<colour, Fn>{static_cast<Fn &&>(fn)});
  }
  template <typename Fn> void fill_uv(Fn &&fn) {
    _fill_uv(s<uv, Fn>{static_cast<Fn &&>(fn)});
  }
  template <typename Fn> void load_atlas(unsigned w, unsigned h, Fn &&fn) {
    _load_atlas(w, h, s<u8_rgba, Fn>{static_cast<Fn &&>(fn)});
  }

  void process_event(const casein::event &e) {
    switch (e.type()) {
    case casein::CREATE_WINDOW:
      setup(*e.as<casein::events::create_window>());
      break;
    case casein::RESIZE_WINDOW: {
      const auto &[w, h, live] = *e.as<casein::events::resize_window>();
      resize(w, h);
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
