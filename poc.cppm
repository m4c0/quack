#ifndef LECO_TARGET_WASM
#pragma leco app
#endif

export module poc;

import quack;
import sith;
import sitime;
import vee;
import voo;
import vapp;

extern "C" float sinf(float);

struct u8_rgba {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};
static voo::h2l_image gen_atlas(vee::physical_device pd) {
  voo::h2l_image res { pd, 16, 32, VK_FORMAT_R8G8B8A8_SRGB };

  voo::mapmem m { res.host_memory() };
  auto * img = static_cast<u8_rgba *>(*m);
  for (auto i = 0; i < 16 * 16; i++) {
    auto x = (i / 16) % 2;
    auto y = (i % 16) % 2;
    unsigned char b = (x ^ y) == 0 ? 255 : 0;

    img[i] = { 255, 255, 255, 0 };
    img[i + 256] = { b, b, b, 128 };
  }

  return res;
}
static void update_data(quack::instance *& i) {
  static sitime::stopwatch time {};

  float a = sinf(time.millis() / 1000.0f) * 0.5f + 0.5f;
  *i++ = (quack::instance) {
    .position { 0, 0 },
    .size { 1, 1 },
    .colour { 0.0f, 0.6f, 0.9f, 0.5f },
    .multiplier { 1, 1, 1, 1 },
  };
  *i++ = (quack::instance) {
    .position { 0.25f, 0.25f },
    .size { 0.5f, 0.5f },
    .uv0 { 0, 0 },
    .uv1 { 1, 1 },
    .colour { 0.25f, 0.0f, 0.1f, a },
    .multiplier { 1, 1, 1, 1 },
    .rotation { 5, 0.5, 0.5 },
  };
}

constexpr const auto max_batches = 100;
struct renderer : public vapp {
  void run() override {
    main_loop("quack", [&](auto & dq, auto & sw) {
      quack::pipeline_stuff ps { dq, max_batches };
      quack::buffer_updater u { &dq, 2, &update_data };

      sith::run_guard rg { &u }; // For animation

      quack::image_updater a { &dq, &ps, &gen_atlas };

      // For a custom sampler
      a.smp() = vee::create_sampler(vee::linear_sampler);
      a.run_once();

      quack::upc rpc {
        .grid_pos = { 0.5f, 0.5f },
        .grid_size = { 1.0f, 1.0f },
      };

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&] {
          auto scb = sw.cmd_render_pass({
              .clear_colours { vee::clear_colour({}) },
          });
          quack::run(&ps, {
              .sw = &sw,
              .pc = &rpc,
              .inst_buffer = u.data().local_buffer(),
              .atlas_dset = a.dset(),
              .count = 1,
          });
          quack::run(&ps, {
              .sw = &sw,
              .pc = &rpc,
              .inst_buffer = u.data().local_buffer(),
              .atlas_dset = a.dset(),
              .count = 1,
              .scissor = { { 0.25f }, { 0.5f } },
          });
          quack::run(&ps, {
              .sw = &sw,
              .pc = &rpc,
              .inst_buffer = u.data().local_buffer(),
              .atlas_dset = a.dset(),
              .count = 1,
              .first = 1,
              .scissor = { { 0.25f }, { 0.5f }, &rpc },
          });
        });
      });
    });
  }
} r;

