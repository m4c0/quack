export module quack:v_stage;
import :v_per_device;
import :objects;
import vee;

namespace quack {
class stage_image {
  const per_device *dev;

  vee::buffer ts_buf{};
  vee::device_memory ts_mem{};
  decltype(nullptr) ts_bind{};

  vee::image t_img{};
  vee::device_memory t_mem{};
  decltype(nullptr) t_bind{};
  vee::image_view t_iv{};

  unsigned m_w{};
  unsigned m_h{};
  bool m_dirty;

public:
  explicit stage_image(const per_device *d) : dev{d} {}

  [[nodiscard]] constexpr const auto &image_view() const noexcept {
    return t_iv;
  }

  bool resize_image(unsigned w, unsigned h) {
    if (w == m_w && h == m_h)
      return false;

    // If we need this in higher rate, then it's better to go with multiple
    // buffers
    vee::device_wait_idle();

    ts_buf = vee::create_transfer_src_buffer(w * h * sizeof(u8_rgba));
    ts_mem = vee::create_host_buffer_memory(dev->physical_device(), *ts_buf);
    ts_bind = vee::bind_buffer_memory(*ts_buf, *ts_mem);

    t_img = vee::create_srgba_image({w, h});
    t_mem = vee::create_local_image_memory(dev->physical_device(), *t_img);
    t_bind = vee::bind_image_memory(*t_img, *t_mem);
    t_iv = vee::create_srgba_image_view(*t_img);

    m_w = w;
    m_h = h;
    return true;
  }
  void load_image(const filler<u8_rgba> &fn) {
    vee::map_memory<u8_rgba>(*ts_mem, fn);
    m_dirty = true;
  }

  void build_commands(vee::command_buffer cb) {
    if (!m_dirty)
      return;

    vee::cmd_pipeline_barrier(cb, *t_img, vee::from_host_to_transfer);
    vee::cmd_copy_buffer_to_image(cb, {m_w, m_h}, *ts_buf, *t_img);
    vee::cmd_pipeline_barrier(cb, *t_img, vee::from_transfer_to_fragment);

    m_dirty = false;
  }
};
} // namespace quack
