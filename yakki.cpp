module quack;
import casein;
import dotz;
import hai;
import jute;
import sith;
import traits;
import vee;
import voo;

using namespace quack;
using namespace quack::yakki;

namespace quack::yakki {
  void (*on_start)(resources *) {};
  void (*on_frame)(renderer *) {};
  dotz::vec4 clear_colour { 0, 0, 0, 1 };

} // namespace quack::yakki

namespace {
  class buf : public quack::yakki::buffer {
    buffer_updater m_buffer {};
    upc m_pc {};
    quack::scissor m_scissor {};
    sith::run_guard m_guard {};

  public:
    constexpr buf() = default;
    explicit buf(buffer_updater b) : m_buffer { traits::move(b) } {}

    [[nodiscard]] upc & pc() override { return m_pc; }
    [[nodiscard]] quack::scissor & scissor() override { return m_scissor; }

    [[nodiscard]] constexpr auto local_buffer() const { return m_buffer.data().local_buffer(); }
    [[nodiscard]] unsigned count() const override { return m_buffer.count(); }

    [[nodiscard]] dotz::vec2 mouse_pos() const override {
      auto aspect = casein::window_size.x / casein::window_size.y;
      auto upc = quack::adjust_aspect(m_pc, aspect);
      auto wnd = casein::mouse_pos / casein::window_size;
      auto rel = wnd * 2.0 - 1.0f;
      return rel * upc.grid_size + upc.grid_pos;
    }

    void start() override { m_guard = sith::run_guard { &m_buffer }; }
    void run_once() override { m_buffer.run_once(); }
  };
  class img : public quack::yakki::image {
    image_updater m_image {};

  public:
    constexpr img() = default;
    explicit img(image_updater b) : m_image { traits::move(b) } {}

    [[nodiscard]] constexpr auto dset() const { return m_image.dset(); }
  };

  class resources : public quack::yakki::resources {
    voo::device_and_queue * m_dq;
    pipeline_stuff * m_ps;

    hai::varray<img> m_imgs { 16 };
    hai::varray<buf> m_bufs { 128 };

  public:
    constexpr resources(voo::device_and_queue * dq, pipeline_stuff * ps) : m_dq { dq }, m_ps { ps } {}

    [[nodiscard]] yakki::image * image(jute::view name) override {
      m_imgs.push_back(img { image_updater { m_dq, m_ps, voo::load_sires_image(name) } });
      return &m_imgs.back();
    }
    [[nodiscard]] yakki::buffer * buffer(unsigned size, buffer_fn_t && fn) override {
      m_bufs.push_back(buf { buffer_updater { m_dq, size, fn } });
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

    void run(buffer * yb, image * yi, unsigned count, unsigned first = 0) override {
      auto b = static_cast<buf *>(yb);
      auto i = static_cast<img *>(yi);
      quack::run(m_ps, {
          .sw = m_sw,
          .scb = m_cb,
          .pc = &b->pc(),
          .inst_buffer = b->local_buffer(),
          .atlas_dset = i->dset(),
          .count = count,
          .first = first,
          .scissor = b->scissor(),
      });
    }
    void run(buffer * yb, image * i) override {
      auto b = static_cast<buf *>(yb);
      run(b, i, b->count());
    }
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
