#pragma leco app
export module poc;

import casein;
import silog;
import quack;
import vee;
import voo;

void atlas_image(quack::u8_rgba *img) {
  for (auto i = 0; i < 16 * 16; i++) {
    auto x = (i / 16) % 2;
    auto y = (i % 16) % 2;
    unsigned char b = (x ^ y) == 0 ? 255 : 0;

    img[i] = {255, 255, 255, 0};
    img[i + 256] = {b, b, b, 128};
  }
}

constexpr const auto max_batches = 100;
class renderer : public voo::casein_thread {
  quack::instance_batch *m_ib;

public:
  void setup_batch() {
    auto lck = wait_init();

    m_ib->map_all([](auto p) {
      auto &[cs, ms, ps, us] = p;
      ps[1] = {{0.25, 0.25}, {0.5, 0.5}};
      cs[1] = {0.25, 0, 0.1, 1.0};
      us[1] = {};
      ms[1] = {1, 1, 1, 1};
    });
  }

  void run() override {
    voo::device_and_queue dq{"quack", native_ptr()};

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      quack::pipeline_stuff ps{dq, sw, max_batches};
      auto ib = ps.create_batch(2);

      ib.load_atlas(16, 32, atlas_image);
      ib.map_positions([](auto *ps) { ps[0] = {{0, 0}, {1, 1}}; });
      ib.map_colours([](auto *cs) { cs[0] = {0, 0, 0.1, 1.0}; });
      ib.map_uvs([](auto *us) { us[0] = {}; });
      ib.map_multipliers([](auto *ms) { ms[0] = {1, 1, 1, 1}; });
      ib.center_at(0.5, 0.5);
      ib.set_count(2);
      ib.set_grid(1, 1);

      m_ib = &ib;
      release_init_lock();
      extent_loop(dq, sw, [&] {
        ib.submit_buffers(dq.queue());

        sw.one_time_submit(dq, [&](auto &pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          ps.run(*scb, ib);
        });
      });
    }
  }

  static auto &instance() {
    static renderer r{};
    return r;
  }
};

class updater : public voo::casein_thread {
public:
  void run() override { renderer::instance().setup_batch(); }

  static auto &instance() {
    static updater r{};
    return r;
  }
};

extern "C" void casein_handle(const casein::event &e) {
  updater::instance().handle(e);
  renderer::instance().handle(e);
  quack::mouse_tracker::instance().handle(e);
}
