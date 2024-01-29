#pragma leco app
export module poc;

import casein;
import silog;
import quack;
import sith;
import sitime;
import vee;
import voo;

static void build_atlas_image(voo::h2l_image &atlas) {
  voo::mapmem m{atlas.host_memory()};
  auto *img = static_cast<quack::u8_rgba *>(*m);
  for (auto i = 0; i < 16 * 16; i++) {
    auto x = (i / 16) % 2;
    auto y = (i % 16) % 2;
    unsigned char b = (x ^ y) == 0 ? 255 : 0;

    img[i] = {255, 255, 255, 0};
    img[i + 256] = {b, b, b, 128};
  }
}

extern "C" float sinf(float);
constexpr const auto max_batches = 100;
class renderer : public voo::casein_thread {
  quack::instance_batch *m_ib;
  sith::memfn_thread<renderer> m_update_thread{this, &renderer::setup_batch};

  void setup_batch(sith::thread *) {
    sitime::stopwatch time{};

    wait_init();
    while (!interrupted()) {
      if (!m_ib)
        continue;

      float a = sinf(time.millis() / 1000.0f) * 0.5f + 0.5f;
      m_ib->map_all([a](auto p) {
        auto &[cs, ms, ps, us] = p;
        ps[1] = {{0.25, 0.25}, {0.5, 0.5}};
        cs[1] = {0.25, 0, 0.1, a};
        us[1] = {{0, 0}, {1, 1}};
        ms[1] = {1, 1, 1, 1};
      });
    }
  }

  renderer() { m_update_thread.start(); }

public:
  void run() override {
    voo::device_and_queue dq{"quack", native_ptr()};

    voo::h2l_image atlas{dq.physical_device(), 16, 32};

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      quack::pipeline_stuff ps{dq, sw, max_batches};
      auto ib = ps.create_batch(2);

      ib.set_atlas(atlas.iv());
      ib.map_positions([](auto *ps) { ps[0] = {{0, 0}, {1, 1}}; });
      ib.map_colours([](auto *cs) { cs[0] = {0, 0, 0.1, 1.0}; });
      ib.map_uvs([](auto *us) { us[0] = {}; });
      ib.map_multipliers([](auto *ms) { ms[0] = {1, 1, 1, 1}; });
      ib.center_at(0.5, 0.5);
      ib.set_count(2);
      ib.set_grid(1, 1);

      build_atlas_image(atlas);

      m_ib = &ib;
      release_init_lock();

      extent_loop(dq, sw, [&] {
        {
          // TODO: this is prone to "tearing" if dataset is larger
          voo::cmd_buf_one_time_submit pcb{sw.command_buffer()};
          atlas.setup_copy(*pcb);
          ib.setup_copy(*pcb);

          auto scb = sw.cmd_render_pass(pcb);
          ib.build_commands(*pcb);
          ps.run(*scb, ib);
        }
        sw.queue_submit(dq);
      });

      m_ib = nullptr;
    }
  }

  static auto &instance() {
    static renderer r{};
    return r;
  }
};

extern "C" void casein_handle(const casein::event &e) {
  renderer::instance().handle(e);
  quack::mouse_tracker::instance().handle(e);
}
