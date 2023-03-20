export module quack;
export import :objects;
import casein;
import hai;

export namespace quack {
class pimpl;
class renderer {
  hai::uptr<pimpl> m_pimpl;

  void _fill_colour(const filler<colour> &);
  void _fill_pos(const filler<pos> &);

public:
  renderer(const params &p);
  ~renderer();

  void setup(casein::native_handle_t);
  void repaint(unsigned i_count);
  void quit();

  template <typename Fn> void fill_pos(Fn &&fn) {
    class s : public filler<pos> {
      Fn m;

    public:
      constexpr s(Fn &&fn) : m{fn} {}
      void operator()(pos *c) const noexcept { m(c); }
    };
    _fill_pos(s{static_cast<Fn &&>(fn)});
  }
  template <typename Fn> void fill_colour(Fn &&fn) {
    class s : public filler<colour> {
      Fn m;

    public:
      constexpr s(Fn &&fn) : m{fn} {}
      void operator()(colour *c) const noexcept { m(c); }
    };
    _fill_colour(s{static_cast<Fn &&>(fn)});
  }
};
} // namespace quack
