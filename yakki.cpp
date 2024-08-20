module quack;
import dotz;
import voo;

using namespace quack::yakki;

namespace quack::yakki {
  void (*on_start)(resources *) {};
  void (*on_frame)(renderer *) {};
  dotz::vec4 clear_colour { 0, 0, 0, 1 };
}

namespace {
  class thread : public voo::casein_thread {
    static constexpr const auto max_dsets = 16;

  public:
    void run() override {
      voo::device_and_queue dq {};

      quack::pipeline_stuff ps { dq, max_dsets };
      resources r { &dq, &ps };

      on_start(&r);

      while (!interrupted()) {
        voo::swapchain_and_stuff sw { dq };

        extent_loop(dq.queue(), sw, [&] {
          sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
            const auto cc = clear_colour;
            auto scb = sw.cmd_render_pass({
                .command_buffer = *pcb,
                .clear_color = { { cc.x, cc.y, cc.z, cc.w } },
            });

            renderer r { &sw, &ps, *scb };
            on_frame(&r);
          });
        });
      }
    }
  } t;
}
