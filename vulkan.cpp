module quack;
import hai;
import casein;
import vee;

namespace quack {
class pimpl {
public:
  explicit pimpl(unsigned max_quad) {}
};

renderer::renderer(unsigned max_quad)
    : m_pimpl{hai::uptr<pimpl>::make(max_quad)} {}
renderer::~renderer() = default;

void renderer::update(const filler &g) {}
void renderer::repaint() {}
void renderer::setup(casein::native_handle_t nptr) { vee::initialise(); }
void renderer::quit() {
  vee::device_wait_idle();
  m_pimpl = {};
}
} // namespace quack
