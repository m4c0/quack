export module quack:per_inflight;
import :per_extent;
import traits;
import vee;

namespace quack {
class per_inflight {
  vee::semaphore img_available_sema = vee::create_semaphore();
  vee::semaphore rnd_finished_sema = vee::create_semaphore();
  vee::fence f = vee::create_fence_signaled();

public:
  [[nodiscard]] constexpr auto image_available_sema() const noexcept {
    return *img_available_sema;
  }
  [[nodiscard]] constexpr auto render_finished_sema() const noexcept {
    return *rnd_finished_sema;
  }

  void wait_and_reset_fence() const { vee::wait_and_reset_fence(*f); }

  void submit(vee::queue q, vee::command_buffer primary_cb) const {
    vee::queue_submit({
        .queue = q,
        .fence = *f,
        .command_buffer = primary_cb,
        .wait_semaphore = *img_available_sema,
        .signal_semaphore = *rnd_finished_sema,
    });
  }
};

class inflight_pair {
  per_inflight front{};
  per_inflight back{};

public:
  [[nodiscard]] auto &flip() {
    auto tmp = traits::move(front);
    front = traits::move(back);
    back = traits::move(tmp);

    back.wait_and_reset_fence();
    return back;
  }
};
} // namespace quack
