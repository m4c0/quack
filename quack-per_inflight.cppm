export module quack:per_inflight;
import :per_extent;
import traits;
import vee;

namespace quack {
class per_inflight {
  vee::semaphore img_available_sema = vee::create_semaphore();
  vee::semaphore rnd_finished_sema = vee::create_semaphore();
  vee::fence f = vee::create_fence_signaled();
  vee::command_buffer cb;

public:
  explicit per_inflight(const vee::command_pool *pool)
      : cb{vee::allocate_secondary_command_buffer(**pool)} {}

  [[nodiscard]] constexpr auto command_buffer() const noexcept { return cb; }
  [[nodiscard]] constexpr auto render_finished_sema() const noexcept {
    return *rnd_finished_sema;
  }

  [[nodiscard]] auto wait_and_takeoff(const per_extent *ext) const {
    vee::wait_and_reset_fence(*f);
    return vee::acquire_next_image(ext->swapchain(), *img_available_sema);
  }

  void submit(const per_device *dev,
              const vee::command_buffer primary_cb) const {
    vee::queue_submit({
        .queue = dev->queue(),
        .fence = *f,
        .command_buffer = primary_cb, // TODO: "per_frame"
        .wait_semaphore = *img_available_sema,
        .signal_semaphore = *rnd_finished_sema,
    });
  }
};

class inflight_pair {
  vee::command_pool cp;

  per_inflight front{&cp};
  per_inflight back{&cp};

public:
  explicit inflight_pair(const per_device *dev)
      : cp{vee::create_command_pool(dev->queue_family())} {}

  [[nodiscard]] auto &flip() {
    auto tmp = traits::move(front);
    front = traits::move(back);
    back = traits::move(tmp);

    return back;
  }
};
} // namespace quack
