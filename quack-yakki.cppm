export module quack:yakki;
import :pipeline_stuff;
import :objects;
import :upc;
import :updater;
import hai;
import jute;
import sith;
import traits;
import vee;
import voo;

export namespace quack::yakki {
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

  void (*on_start)(resources *);
  void (*on_frame)(renderer *);

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
