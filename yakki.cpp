module quack;
import casein;
import dotz;
import hai;
import jute;
import vee;
import voo;

using namespace quack;
using namespace quack::yakki;

namespace quack::yakki {
  void (*on_start)(resources *) {};
  void (*on_frame)(renderer *) {};
  dotz::vec4 clear_colour { 0, 0, 0, 1 };

  dotz::vec2 buffer::mouse_pos() const {
    auto aspect = casein::window_size.x / casein::window_size.y;
    auto upc = quack::adjust_aspect(m_pc, aspect);
    auto wnd = casein::mouse_pos / casein::window_size;
    auto rel = wnd * 2.0 - 1.0f;
    return rel * upc.grid_size + upc.grid_pos;
  }
} // namespace quack::yakki

namespace {
  class resources : public quack::yakki::resources {
    voo::device_and_queue * m_dq;
    pipeline_stuff * m_ps;

    hai::varray<yakki::image> m_imgs { 16 };
    hai::varray<yakki::buffer> m_bufs { 128 };

  public:
    constexpr resources(voo::device_and_queue * dq, pipeline_stuff * ps) : m_dq { dq }, m_ps { ps } {}

    [[nodiscard]] yakki::image * image(jute::view name) override {
      m_imgs.push_back(yakki::image { m_dq, m_ps, voo::load_sires_image(name) });
      return &m_imgs.back();
    }
    [[nodiscard]] yakki::buffer * buffer(unsigned size, buffer_fn_t && fn) override {
      m_bufs.push_back(yakki::buffer { buffer_updater { m_dq, size, fn } });
      return &m_bufs.back();
    }
  };

  class renderer : public quack::yakki::renderer {
    voo::swapchain_and_stuff * m_sw;
    pipeline_stuff * m_ps;
    vee::command_buffer m_cb;

  public:
    constexpr renderer(voo::swapchain_and_stuff * sw, pipeline_stuff * ps, vee::command_buffer cb)
        : m_sw { sw }
        , m_ps { ps }
        , m_cb { cb } {}

    void run(buffer * b, image * i, unsigned count, unsigned first = 0) override {
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
    void run(buffer * b, image * i) override { run(b, i, b->count()); }
  };

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
            const auto cc = clear_colour;
            auto scb = sw.cmd_render_pass({
                .command_buffer = *pcb,
                .clear_color = { { cc.x, cc.y, cc.z, cc.w } },
            });

            renderer r { &sw, &ps, *scb };
            on_frame(&r);
          });
        });
      }
    }
  } t;
} // namespace
