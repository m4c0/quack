#ifndef LECO_TARGET_WASM
#pragma leco app
#endif

export module poc;

import casein;
import silog;
import quack;
import sith;
import sitime;
import vee;
import voo;

extern "C" float sinf(float);

struct u8_rgba {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};
static void gen_atlas(voo::h2l_image * i) {
  voo::mapmem m { i->host_memory() };
  auto * img = static_cast<u8_rgba *>(*m);
  for (auto i = 0; i < 16 * 16; i++) {
    auto x = (i / 16) % 2;
    auto y = (i % 16) % 2;
    unsigned char b = (x ^ y) == 0 ? 255 : 0;

    img[i] = { 255, 255, 255, 0 };
    img[i + 256] = { b, b, b, 128 };
  }
}
static void update_data(quack::instance * i) {
  static sitime::stopwatch time {};

  float a = sinf(time.millis() / 1000.0f) * 0.5f + 0.5f;
  i[0] = (quack::instance) {
    .position { 0, 0 },
    .size { 1, 1 },
    .colour { 0.0f, 0.0f, 0.1f, 1.0f },
    .multiplier { 1, 1, 1, 1 },
  };
  i[1] = (quack::instance) {
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
class renderer : public voo::casein_thread {
public:
  void run() override {
    voo::device_and_queue dq { "quack" };

    quack::pipeline_stuff ps { dq, max_batches };
    quack::buffer_updater u { &dq, 2, &update_data };

    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq };

      auto a = voo::updater { dq.queue(), &gen_atlas, dq.physical_device(), 16U, 32U };
      a.run_once();

      auto smp = vee::create_sampler(vee::nearest_sampler);
      auto dset = ps.allocate_descriptor_set(a.data().iv(), *smp);

      quack::upc rpc {
        .grid_pos = { 0.5f, 0.5f },
        .grid_size = { 1.0f, 1.0f },
      };

      extent_loop(dq.queue(), sw, [&] {
        auto upc = quack::adjust_aspect(rpc, sw.aspect());
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_set_viewport(*scb, sw.extent());
          vee::cmd_set_scissor(*scb, sw.extent());
          vee::cmd_bind_vertex_buffers(*scb, 1, u.data().local_buffer());
          ps.cmd_bind_descriptor_set(*scb, dset);
          ps.cmd_push_vert_frag_constants(*scb, upc);
          ps.run(*scb, 2);
        });
      });
    }
  }
} r;
