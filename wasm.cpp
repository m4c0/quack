module quack;
import :objects;
import casein;
import hai;

extern "C" void quack_fill_colour(float r, float g, float b);
extern "C" void quack_fill_rect(unsigned x, unsigned y, unsigned w, unsigned h);

namespace quack {
class pimpl {
  hai::holder<quad[]> m_map;

public:
  pimpl(unsigned qs) : m_map{hai::holder<quad[]>::make(qs)} {}

  [[nodiscard]] constexpr auto ptr() noexcept { return *m_map; }
};

renderer::renderer(unsigned max_quad)
    : m_pimpl{hai::uptr<pimpl>::make(max_quad)} {}
renderer::~renderer() = default;

void renderer::update(const filler &g) { g(m_pimpl->ptr()); }
void renderer::repaint() {
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
