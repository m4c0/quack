#pragma leco app
export module poc;

import casein;
import silog;
import quack;
import sith;
import sitime;
import vee;
import voo;

class atlas : public voo::updater_thread<voo::h2l_image> {
  void update_data(voo::h2l_image *i) override {
    voo::mapmem m{i->host_memory()};
    auto *img = static_cast<quack::u8_rgba *>(*m);
    for (auto i = 0; i < 16 * 16; i++) {
      auto x = (i / 16) % 2;
      auto y = (i % 16) % 2;
      unsigned char b = (x ^ y) == 0 ? 255 : 0;

      img[i] = {255, 255, 255, 0};
      img[i + 256] = {b, b, b, 128};
    }
  }

public:
  atlas(voo::device_and_queue *dq)
      : updater_thread{dq->queue(),
                       voo::h2l_image{dq->physical_device(), 16, 32}} {
    run_once();
  }
};

extern "C" float sinf(float);
class updater : public quack::instance_batch_thread {
  sitime::stopwatch time{};

  void update_data(all p) override {
    float a = sinf(time.millis() / 1000.0f) * 0.5f + 0.5f;
    auto &[cs, ms, ps, us] = p;
    ps[1] = {{0.25, 0.25}, {0.5, 0.5}};
    cs[1] = {0.25, 0, 0.1, a};
    us[1] = {{0, 0}, {1, 1}};
    ms[1] = {1, 1, 1, 1};
  }

public:
  updater(voo::device_and_queue *dq, quack::pipeline_stuff &ps)
      : instance_batch_thread{dq->queue(), ps.create_batch(2)} {
    auto &ib = data();
    ib.map_positions([](auto *ps) { ps[0] = {{0, 0}, {1, 1}}; });
    ib.map_colours([](auto *cs) { cs[0] = {0, 0, 0.1, 1.0}; });
    ib.map_uvs([](auto *us) { us[0] = {}; });
    ib.map_multipliers([](auto *ms) { ms[0] = {1, 1, 1, 1}; });
  }
};

constexpr const auto max_batches = 100;
class renderer : public voo::casein_thread {
public:
  void run() override {
    voo::device_and_queue dq{"quack", native_ptr()};

    quack::pipeline_stuff ps{dq, max_batches};
    updater u{&dq, ps};

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};
      sith::run_guard ru{&u};

      atlas a{&dq};

      auto smp = vee::create_sampler(vee::nearest_sampler);
      auto dset = ps.allocate_descriptor_set(a.data().iv(), *smp);

      quack::upc rpc{
          .grid_pos = {0.5f, 0.5f},
          .grid_size = {1.0f, 1.0f},
      };

      extent_loop(dq.queue(), sw, [&] {
        auto upc = quack::adjust_aspect(rpc, sw.aspect());
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_set_viewport(*scb, sw.extent());
          vee::cmd_set_scissor(*scb, sw.extent());
          u.data().build_commands(*pcb);
          ps.cmd_bind_descriptor_set(*scb, dset);
          ps.cmd_push_vert_frag_constants(*scb, upc);
          ps.run(*scb, 2);
        });
      });
    }
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static renderer r{};
  r.handle(e);
}
