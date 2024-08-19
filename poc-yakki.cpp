#ifndef LECO_TARGET_WASM
#pragma leco app
#pragma leco add_resource "nasa-jupiter.png"
#endif

import casein;
import silog;
import quack;
import sith;
import sitime;
import vee;
import voo;

extern "C" float cosf(float);
extern "C" float sinf(float);

static void back(quack::instance *& i) {
  *i++ = {
    .position = { -1 },
    .size = { 2 },
    .colour = { 0, 0, 0, 1 },
  };
}
static void spiral(quack::instance *& i) {
  static sitime::stopwatch time {};

  auto t = time.millis() / 1000.f;
  for (auto n = 0; n < 100; n++) {
    auto a = n * 6.0f * 3.14159265f / 100.0f;
    auto x = a * 0.5f * cosf(a - t);
    auto y = a * 0.5f * sinf(a - t);

    *i++ = (quack::instance) {
      .position { x, y },
      .size { 0.5f, 0.5f },
      .uv0 { 0, 0 },
      .uv1 { 1, 1 },
      .multiplier { 1, 1, 1, 1 },
    };
  }
}

constexpr const auto max_batches = 100;
class renderer : public voo::casein_thread {
public:
  void run() override {
    voo::device_and_queue dq { "quack" };

    quack::pipeline_stuff ps { dq, max_batches };

    quack::image_updater a { &dq, &ps, voo::load_sires_image("nasa-jupiter.png") };

    quack::buffer_updater bg { &dq, 100, &back };
    quack::upc rpc_back {
      .grid_pos = { 0 },
      .grid_size = { 1 },
    };

    quack::buffer_updater u { &dq, 100, &spiral };
    sith::run_guard rg { &u }; // For animation
    quack::upc rpc {
      .grid_pos = { 0 },
      .grid_size = { 12 },
    };

    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq };

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          ps.run({
              .sw = &sw,
              .scb = *scb,
              .pc = &rpc_back,
              .inst_buffer = bg.data().local_buffer(),
              .atlas_dset = a.dset(),
              .count = 1,
          });
          ps.run({
              .sw = &sw,
              .scb = *scb,
              .pc = &rpc,
              .inst_buffer = u.data().local_buffer(),
              .atlas_dset = a.dset(),
              .count = 100,
          });
        });
      });
    }
  }
} r;
