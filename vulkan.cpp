module quack;
import casein;

namespace quack {
class pimpl {};

renderer::renderer(unsigned max_quad) {}
renderer::~renderer() = default;

void renderer::update(const filler &g) {}
void renderer::repaint() {}
void renderer::setup(casein::native_handle_t) {}
void renderer::quit() {}
} // namespace quack
