export module quack:per_extent;
import vee;

namespace quack {
class per_extent {
  vee::physical_device m_pd;
  vee::surface::type m_s;

  vee::extent extent = vee::get_surface_capabilities(m_pd, m_s).currentExtent;

  vee::swapchain swc = vee::create_swapchain(m_pd, m_s);

  vee::render_pass rp = vee::create_render_pass(m_pd, m_s);

  vee::image d_img = vee::create_depth_image(m_pd, m_s);
  vee::device_memory d_mem = vee::create_local_image_memory(m_pd, *d_img);
  decltype(nullptr) d_bind = vee::bind_image_memory(*d_img, *d_mem);
  vee::image_view d_iv = vee::create_depth_image_view(*d_img);

public:
  per_extent(vee::physical_device pd, vee::surface::type s)
      : m_pd{pd}, m_s{s} {}

  [[nodiscard]] constexpr auto extent_2d() const noexcept { return extent; }
  [[nodiscard]] constexpr auto render_pass() const noexcept { return *rp; }
  [[nodiscard]] constexpr auto swapchain() const noexcept { return *swc; }

  [[nodiscard]] auto create_framebuffer(const vee::image_view &iv) const {
    vee::fb_params fbp{
        .physical_device = m_pd,
        .surface = m_s,
        .render_pass = *this->rp,
        .image_buffer = *iv,
        .depth_buffer = *this->d_iv,
    };
    return vee::create_framebuffer(fbp);
  }
};
} // namespace quack
