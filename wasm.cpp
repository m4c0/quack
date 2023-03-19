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

public:
  pimpl(unsigned qs)
      : m_colour{hai::holder<colour[]>::make(qs)}, m_pos{
                                                       hai::holder<pos[]>::make(
                                                           qs)} {}

  [[nodiscard]] constexpr auto colours() noexcept { return *m_colour; }
  [[nodiscard]] constexpr auto positions() noexcept { return *m_pos; }
};

renderer::renderer(const params &p)
    : m_pimpl{hai::uptr<pimpl>::make(p.max_quads)} {}
renderer::~renderer() = default;

void renderer::fill_colour(const filler<colour> &g) { g(m_pimpl->colours()); }
void renderer::fill_pos(const filler<pos> &g) { g(m_pimpl->positions()); }
void renderer::repaint(unsigned i_count) {
  /*
  for (auto i = 0; i < ecs::grid_cells; i++) {
    const auto &b = m_map[i];
    const auto w = 800 / ecs::grid_w;
    const auto h = 600 / ecs::grid_h;
    const auto x = w * (i % ecs::grid_w);
    const auto y = h * (i / ecs::grid_w);

    quack_fill_colour(b.r, b.g, b.b);
    quack_fill_rect(x, y, w, h);
  }
  */
}
void renderer::setup(casein::native_handle_t) {}
void renderer::quit() {}
} // namespace quack
