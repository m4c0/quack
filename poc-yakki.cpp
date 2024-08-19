#ifndef LECO_TARGET_WASM
#pragma leco app
#pragma leco add_resource "nasa-jupiter.png"
#endif

import casein;
import hai;
import jute;
import quack;
import sith;
import sitime;
import vee;
import voo;

namespace quack::yakki {
  class buffer {
    buffer_updater m_buffer {};
    upc m_pc {};
    sith::run_guard m_guard {};

  public:
    constexpr buffer() = default;
    buffer(buffer_updater b) : m_buffer { traits::move(b) } {}

    [[nodiscard]] constexpr auto & pc() { return m_pc; }

    [[nodiscard]] constexpr auto local_buffer() const { return m_buffer.data().local_buffer(); }

    void start() { m_guard = sith::run_guard { &m_buffer }; }
  };

  class resources {
    voo::device_and_queue * m_dq;
    pipeline_stuff * m_ps;

    hai::varray<image_updater> m_imgs { 16 };
    hai::varray<buffer> m_bufs { 128 };

  public:
    constexpr resources(voo::device_and_queue * dq, pipeline_stuff * ps) : m_dq { dq }, m_ps { ps } {}

    [[nodiscard]] auto * image(jute::view name) {
      m_imgs.push_back(image_updater { m_dq, m_ps, voo::load_sires_image(name) });
      return &m_imgs.back();
    }
    [[nodiscard]] auto buffer(unsigned size, auto && fn) {
      m_bufs.push_back(yakki::buffer { buffer_updater { m_dq, size, fn } });
      return &m_bufs.back();
    }
  };

  class renderer {
    voo::swapchain_and_stuff * m_sw;
    pipeline_stuff * m_ps;
    vee::command_buffer m_cb;

  public:
    constexpr renderer(voo::swapchain_and_stuff * sw, pipeline_stuff * ps, vee::command_buffer cb)
        : m_sw { sw }
        , m_ps { ps }
        , m_cb { cb } {}

    void run(buffer * b, image_updater * i, unsigned count, unsigned first = 0) {
      m_ps->run({
          .sw = m_sw,
          .scb = m_cb,
          .pc = &b->pc(),
          .inst_buffer = b->local_buffer(),
          .atlas_dset = i->dset(),
          .count = count,
          .first = first,
      });
    }
  };

  hai::fn<void, resources *> on_start {};
  hai::fn<void, renderer *> on_frame {};

  class thread : public voo::casein_thread {
    static constexpr const auto max_dsets = 16;

  public:
    void run() override {
      voo::device_and_queue dq {};

      pipeline_stuff ps { dq, max_dsets };
      resources r { &dq, &ps };

      on_start(&r);

      while (!interrupted()) {
        voo::swapchain_and_stuff sw { dq };

        extent_loop(dq.queue(), sw, [&] {
          sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
            auto scb = sw.cmd_render_pass(pcb);

            renderer r { &sw, &ps, *scb };
            on_frame(&r);
          });
        });
      }
    }
  };
} // namespace quack::yakki

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

static quack::yakki::buffer * g_bg;
static quack::yakki::buffer * g_u;
static quack::image_updater * g_a;
static void on_start(quack::yakki::resources * r) {
  g_bg = r->buffer(100, back);
  g_bg->pc() = {
    .grid_pos = { 0 },
    .grid_size = { 1 },
  };

  g_u = r->buffer(100, spiral);
  g_u->pc() = {
    .grid_pos = { 0 },
    .grid_size = { 12 },
  };
  g_u->start();

  g_a = r->image("nasa-jupiter.png");
}
static void on_frame(quack::yakki::renderer * r) {
  r->run(g_bg, g_a, 1);
  r->run(g_u, g_a, 100);
}

struct init : quack::yakki::thread {
  init() : thread() {
    quack::yakki::on_start = on_start;
    quack::yakki::on_frame = on_frame;
  }
} r;
