export module quack:v_stage;
import :v_per_device;
import vee;

namespace quack {
class stage_image {
  const per_device *dev;

  vee::buffer ts_buf = vee::create_transfer_src_buffer(16 * 16 * sizeof(float));
  vee::device_memory ts_mem =
      vee::create_host_buffer_memory(dev->physical_device(), *ts_buf);
  decltype(nullptr) ts_bind = vee::bind_buffer_memory(*ts_buf, *ts_mem);

  vee::image t_img = vee::create_srgba_image({16, 16});
  vee::device_memory t_mem =
      vee::create_local_image_memory(dev->physical_device(), *t_img);
  decltype(nullptr) t_bind = vee::bind_image_memory(*t_img, *t_mem);
  vee::image_view t_iv = vee::create_srgba_image_view(*t_img);

  bool m_dirty;

public:
  explicit stage_image(const per_device *d) : dev{d} {}

  [[nodiscard]] constexpr const auto &image_view() const noexcept {
    return t_iv;
  }

  void load_image(auto &&fn) {
    vee::map_memory<unsigned char>(*ts_mem, fn);
    m_dirty = true;
  }

  void build_commands(vee::command_buffer cb) {
    if (!m_dirty)
      return;

    vee::cmd_pipeline_barrier(cb, *t_img, vee::from_host_to_transfer);
    vee::cmd_copy_buffer_to_image(cb, {16, 16}, *ts_buf, *t_img);
    vee::cmd_pipeline_barrier(cb, *t_img, vee::from_transfer_to_fragment);

    m_dirty = false;
  }
};
} // namespace quack
