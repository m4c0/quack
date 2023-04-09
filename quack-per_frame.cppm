export module quack:per_frame;
import :per_extent;
import vee;

namespace quack {
class per_frame {
  const per_extent *ext;
  vee::image_view iv;
  vee::command_buffer cb =
      vee::allocate_primary_command_buffer(ext->primary_command_pool());
  vee::framebuffer fb = ext->create_framebuffer(iv);

public:
  per_frame(const per_device *dev, const per_extent *ext, auto img)
      : ext{ext}, iv{vee::create_rgba_image_view(img, dev->physical_device(),
                                                 dev->surface())} {}

  [[nodiscard]] auto one_time_submit(auto pfn, auto fn) {
    vee::begin_cmd_buf_one_time_submit(cb);
    pfn(cb);

    vee::cmd_begin_render_pass({
        .command_buffer = cb,
        .render_pass = ext->render_pass(),
        .framebuffer = *fb,
        .extent = ext->extent_2d(),
        .clear_color = {0.05, 0.1f, 0.15, 1.0f},
    });

    fn(cb);

    vee::cmd_end_render_pass(cb);
    vee::end_cmd_buf(cb);
    return cb;
  }
};
} // namespace quack
