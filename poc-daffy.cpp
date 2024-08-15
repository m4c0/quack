#pragma leco app
#pragma leco add_resource "nasa-jupiter.png"

import casein;
import quack;
import sith;
import sitime;
import vee;
import voo;

extern "C" float sinf(float);

static void update_data(quack::instance *& i) {
  static sitime::stopwatch time {};

  float a = sinf(time.millis() / 1000.0f) * 30.0f;
  *i++ = (quack::instance) {
    .position { 0, 0 },
    .size { 1, 1 },
    .uv0 { 0, 0 },
    .uv1 { 1, 1 },
    .multiplier { 1, 1, 1, 1 },
    .rotation { a, 0.5, 0.5 },
  };
}

constexpr const auto max_batches = 100;
class renderer : public voo::casein_thread {
public:
  void run() override {
    voo::device_and_queue dq { "quack" };

    quack::pipeline_stuff ps { dq, max_batches };
    quack::buffer_updater u { &dq, 2, update_data };

    sith::run_guard rg { &u }; // For animation

    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq };

      quack::image_updater a { &dq, &ps, [](auto pd) { return voo::load_sires_image("nasa-jupiter.png", pd); } };

      quack::upc rpc {
        .grid_pos = { 0.5f, 0.5f },
        .grid_size = { 4.0f, 4.0f },
      };

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
        auto scb = sw.cmd_render_pass(vee::render_pass_begin {
            .command_buffer = *pcb,
            .clear_color = { { 0, 0, 0, 1 } },
        });
          ps.run({
              .sw = &sw,
              .scb = *scb,
              .pc = &rpc,
              .inst_buffer = u.data().local_buffer(),
              .atlas_dset = a.dset(),
              .count = u.count(),
          });
        });
      });
    }
  }
} r;
