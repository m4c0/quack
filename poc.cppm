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

    m_ib->load_atlas(16, 32, atlas_image);

    m_ib->map_positions([](auto *ps) { ps[0] = {{0, 0}, {1, 1}}; });
    m_ib->map_colours([](auto *cs) { cs[0] = {0, 0, 0.1, 1.0}; });
    m_ib->map_uvs([](auto *us) { us[0] = {}; });
    m_ib->map_multipliers([](auto *ms) { ms[0] = {1, 1, 1, 1}; });
    m_ib->map_all([](auto p) {
      auto &[cs, ms, ps, us] = p;
      ps[1] = {{0.25, 0.25}, {0.5, 0.5}};
      cs[1] = {0.25, 0, 0.1, 1.0};
      us[1] = {};
      ms[1] = {1, 1, 1, 1};
    });
    m_ib->center_at(0.5, 0.5);
    m_ib->set_count(2);
    m_ib->set_grid(1, 1);
  }

  void run() override {
    voo::device_and_queue dq{"quack", native_ptr()};

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      quack::pipeline_stuff ps{dq.physical_device(), max_batches};
      quack::instance_batch ib{dq.physical_device(), dq.command_pool(),
                               ps.pipeline_layout(),
                               ps.allocate_descriptor_set(), 2};

      auto ppl = ps.create_pipeline(sw.render_pass());

      m_ib = &ib;
      release_init_lock();
      extent_loop([&] {
        sw.acquire_next_image();

        ib.build_atlas_commands(dq.queue());

        sw.one_time_submit(dq, [&](auto &pcb) {
          auto scb = sw.cmd_render_pass(pcb);

          vee::cmd_bind_gr_pipeline(*scb, *ppl);

          int n = ib.build_commands(*scb);
          if (n > 0)
            ps.run(*scb, n);
        });

        sw.queue_present(dq);
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
