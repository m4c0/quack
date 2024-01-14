export module quack:renderer;
import :ibatch;
import :pipeline_stuff;
import hai;
import traits;
import vee;
import voo;

export namespace quack {
constexpr const auto max_batches = 100;
class renderer : public voo::casein_thread {
  hai::varray<instance_batch> m_batches{max_batches};
  const voo::device_and_queue *m_dq;
  const pipeline_stuff *m_ps;

public:
  [[nodiscard]] auto *allocate_batch(unsigned max_quads) {
    auto lck = wait_init();
    auto pd = m_dq->physical_device();
    auto cp = m_dq->command_pool();
    auto pl = m_ps->pipeline_layout();
    auto ds = m_ps->allocate_descriptor_set();
    auto res = instance_batch{pd, cp, pl, ds, max_quads};
    m_batches.push_back(traits::move(res));
    return &m_batches[m_batches.size() - 1];
  }

  void run() override {
    voo::device_and_queue dq{"quack", native_ptr()};
    pipeline_stuff ps{dq.physical_device(), max_batches};

    m_dq = &dq;
    m_ps = &ps;
    release_init_lock();

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      auto ppl = ps.create_pipeline(sw.render_pass());

      extent_loop([&] {
        sw.acquire_next_image();

        for (auto &b : m_batches) {
          b.build_atlas_commands(dq.queue());
        }

        sw.one_time_submit(dq, [&](auto &pcb) {
          auto scb = sw.cmd_render_pass(pcb);

          vee::cmd_bind_gr_pipeline(*scb, *ppl);

          for (auto &b : m_batches) {
            int n = b.build_commands(*scb);
            if (n > 0)
              ps.run(*scb, n);
          }
        });

        sw.queue_present(dq);
      });
    }
  }
};
} // namespace quack
