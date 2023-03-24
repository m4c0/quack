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
  void _fill_uv(const filler<uv> &);

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
};

template <auto W, auto H, typename Tp> class grid_renderer {
  renderer m_r;
  Tp m_data[W * H];

public:
  static constexpr const auto width = W;
  static constexpr const auto height = H;
  static constexpr const auto cells = width * height;

  grid_renderer()
      : m_r(params{
            .grid_w = W,
            .grid_h = H,
            .max_quads = W * H,
        }) {}

  void setup(casein::native_handle_t nptr) {
    m_r.setup(nptr);
    m_r.fill_pos([](pos *is) {
      unsigned i = 0;
      for (auto y = 0; y < H; y++) {
        for (auto x = 0; x < W; x++, i++) {
          is[i].x = x;
          is[i].y = y;
        }
      }
    });
  }
  void fill_colour(auto &&fn) {
    m_r.fill_colour([&](auto *c) {
      for (auto i = 0; i < cells; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_uv(auto &&fn) {
    m_r.fill_uv([&](auto *c) {
      for (auto i = 0; i < cells; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void reset_grid() {
    for (auto &d : m_data)
      d = {};
  }
  void repaint() { m_r.repaint(cells); }
  void quit() { m_r.quit(); }

  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) noexcept {
    return m_data[y * W + x];
  }
  [[nodiscard]] constexpr auto &at(unsigned idx) noexcept {
    return m_data[idx];
  }
};
} // namespace quack
