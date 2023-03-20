export module quack;
export import :objects;
import casein;
import hai;

export namespace quack {
class pimpl;
class renderer {
  hai::uptr<pimpl> m_pimpl;

  void _fill_colour(const filler<colour> &);

public:
  renderer(const params &p);
  ~renderer();

  void setup(casein::native_handle_t);
  void fill_pos(const filler<pos> &);
  void repaint(unsigned i_count);
  void quit();

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
