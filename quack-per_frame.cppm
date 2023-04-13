export module quack:per_frame;
import :per_device;
import :per_extent;
import hai;
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

  [[nodiscard]] constexpr const auto command_buffer() const { return cb; }
  [[nodiscard]] constexpr const auto &framebuffer() const { return fb; }
};

class frames {
  hai::holder<hai::uptr<per_frame>[]> m_data{};

public:
  explicit frames(const per_device *dev, const per_extent *ext) {
    auto imgs = vee::get_swapchain_images(ext->swapchain());
    m_data = decltype(m_data)::make(imgs.size());
    for (auto i = 0; i < imgs.size(); i++) {
      auto img = (imgs.data())[i];
      (*m_data)[i] = hai::uptr<per_frame>::make(dev, ext, img);
    }
  }

  [[nodiscard]] auto &operator[](unsigned idx) const noexcept {
    return (*m_data)[idx];
  }
};
} // namespace quack
