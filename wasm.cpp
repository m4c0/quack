module quack;
import :objects;
import casein;
import hai;

extern "C" void quack_fill_colour(float r, float g, float b);
extern "C" void quack_fill_rect(unsigned x, unsigned y, unsigned w, unsigned h);

namespace quack {
class pimpl {
  hai::holder<colour[]> m_colour;
  hai::holder<pos[]> m_pos;
  unsigned m_grid_w;
  unsigned m_grid_h;

public:
  pimpl(const params &p)
      : m_colour{hai::holder<colour[]>::make(p.max_quads)},
        m_pos{hai::holder<pos[]>::make(p.max_quads)}, m_grid_w{p.grid_w},
        m_grid_h{p.grid_h} {}

  [[nodiscard]] constexpr auto colours() noexcept { return *m_colour; }
  [[nodiscard]] constexpr auto positions() noexcept { return *m_pos; }

  [[nodiscard]] constexpr auto grid_w() const noexcept { return m_grid_w; }
  [[nodiscard]] constexpr auto grid_h() const noexcept { return m_grid_h; }
};

renderer::renderer(const params &p) : m_pimpl{hai::uptr<pimpl>::make(p)} {}
renderer::~renderer() = default;

void renderer::_fill_colour(const filler<colour> &g) { g(m_pimpl->colours()); }
void renderer::fill_pos(const filler<pos> &g) { g(m_pimpl->positions()); }
void renderer::repaint(unsigned i_count) {
  for (auto i = 0; i < i_count; i++) {
    const auto &b = m_pimpl->colours()[i];
    quack_fill_colour(b.r, b.g, b.b);

    const auto &p = m_pimpl->positions()[i];
    const auto w = 800 / m_pimpl->grid_w();
    const auto h = 600 / m_pimpl->grid_h();
    const auto x = w * p.x;
    const auto y = h * p.y;
    quack_fill_rect(x, y, w, h);
  }
}
void renderer::setup(casein::native_handle_t) {}
void renderer::quit() {}
} // namespace quack
