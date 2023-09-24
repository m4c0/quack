export module quack:per_extent;
import :per_device;
import vee;

namespace quack {
class per_extent {
  const per_device *dev;

  vee::extent extent =
      vee::get_surface_capabilities(dev->physical_device(), dev->surface())
          .currentExtent;

  vee::swapchain swc =
      vee::create_swapchain(dev->physical_device(), dev->surface());

  vee::command_pool pcp = vee::create_command_pool(dev->queue_family());
  vee::render_pass rp =
      vee::create_render_pass(dev->physical_device(), dev->surface());

  vee::image d_img =
      vee::create_depth_image(dev->physical_device(), dev->surface());
  vee::device_memory d_mem =
      vee::create_local_image_memory(dev->physical_device(), *d_img);
  decltype(nullptr) d_bind = vee::bind_image_memory(*d_img, *d_mem);
  vee::image_view d_iv = vee::create_depth_image_view(*d_img);

public:
  per_extent(const per_device *dev) : dev{dev} {}

  [[nodiscard]] constexpr auto primary_command_pool() const noexcept {
    return *pcp;
  }
  [[nodiscard]] constexpr auto extent_2d() const noexcept { return extent; }
  [[nodiscard]] constexpr auto render_pass() const noexcept { return *rp; }
  [[nodiscard]] constexpr auto swapchain() const noexcept { return *swc; }

  [[nodiscard]] auto create_framebuffer(const vee::image_view &iv) const {
    vee::fb_params fbp{
        .physical_device = dev->physical_device(),
        .surface = dev->surface(),
        .render_pass = *this->rp,
        .image_buffer = *iv,
        .depth_buffer = *this->d_iv,
    };
    return vee::create_framebuffer(fbp);
  }
};
} // namespace quack
