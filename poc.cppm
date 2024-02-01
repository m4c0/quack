#pragma leco app
export module poc;

import casein;
import silog;
import quack;
import sith;
import sitime;
import vee;
import voo;

class atlas : public voo::update_thread {
  voo::h2l_image m_img;

  void build_cmd_buf(vee::command_buffer cb) override {
    voo::cmd_buf_one_time_submit pcb{cb};
    m_img.setup_copy(cb);
  }

public:
  atlas(voo::device_and_queue *dq)
      : update_thread{dq}, m_img{dq->physical_device(), 16, 32} {
    voo::mapmem m{m_img.host_memory()};
    auto *img = static_cast<quack::u8_rgba *>(*m);
    for (auto i = 0; i < 16 * 16; i++) {
      auto x = (i / 16) % 2;
      auto y = (i % 16) % 2;
      unsigned char b = (x ^ y) == 0 ? 255 : 0;

      img[i] = {255, 255, 255, 0};
      img[i + 256] = {b, b, b, 128};
    }
  }

  [[nodiscard]] constexpr auto iv() const noexcept { return m_img.iv(); }

  using update_thread::run_once;
};

extern "C" float sinf(float);
class updater : public voo::update_thread {
  quack::instance_batch m_ib;
  sitime::stopwatch time{};

  void build_cmd_buf(vee::command_buffer cb) override {
    float a = sinf(time.millis() / 1000.0f) * 0.5f + 0.5f;
    m_ib.map_all([a](auto p) {
      auto &[cs, ms, ps, us] = p;
      ps[1] = {{0.25, 0.25}, {0.5, 0.5}};
      cs[1] = {0.25, 0, 0.1, a};
      us[1] = {{0, 0}, {1, 1}};
      ms[1] = {1, 1, 1, 1};
    });

    voo::cmd_buf_one_time_submit pcb{cb};
    m_ib.setup_copy(cb);
  }

public:
  explicit updater(voo::device_and_queue *dq, quack::pipeline_stuff &ps)
      : update_thread{dq}, m_ib{ps.create_batch(2)} {
    m_ib.map_positions([](auto *ps) { ps[0] = {{0, 0}, {1, 1}}; });
    m_ib.map_colours([](auto *cs) { cs[0] = {0, 0, 0.1, 1.0}; });
    m_ib.map_uvs([](auto *us) { us[0] = {}; });
    m_ib.map_multipliers([](auto *ms) { ms[0] = {1, 1, 1, 1}; });
  }

  [[nodiscard]] constexpr auto &batch() noexcept { return m_ib; }

  using update_thread::start;
};

constexpr const auto max_batches = 100;
class renderer : public voo::casein_thread {

public:
  void run() override {
    voo::device_and_queue dq{"quack", native_ptr()};

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      quack::pipeline_stuff ps{dq, sw, max_batches};
      updater u{&dq, ps};

      auto ut = u.start();

      atlas a{&dq};
      a.run_once();

      auto smp = vee::create_sampler(vee::nearest_sampler);
      auto dset = ps.allocate_descriptor_set(a.iv(), *smp);

      quack::upc rpc{
          .grid_pos = {0.5f, 0.5f},
          .grid_size = {1.0f, 1.0f},
      };

      extent_loop(dq, sw, [&] {
        auto upc = quack::adjust_aspect(rpc, sw.aspect());
        sw.queue_one_time_submit(dq, [&](auto pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          auto &ib = u.batch();
          ib.build_commands(*pcb);
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
