export module quack:renderer;
import :ibatch;
import :per_extent;
import :per_frame;
import :per_inflight;
import :pipeline;
import :pipeline_stuff;
import :raii;
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
    inflight_pair ip{};
    pipeline_stuff ps{dq.physical_device(), max_batches};

    m_dq = &dq;
    m_ps = &ps;
    release_init_lock();

    auto pd = dq.physical_device();
    auto s = dq.surface();

    while (!interrupted()) {
      per_extent ext{pd, s};
      pipeline ppl{&ext, &ps};
      frames frms{pd, s, dq.command_pool(), &ext};

      extent_loop([&] {
        auto &inf = ip.flip();
        auto idx = vee::acquire_next_image(ext.swapchain(),
                                           inf.image_available_sema());
        auto &frm = frms[idx];
        auto cb = frm->command_buffer();

        for (auto &b : m_batches) {
          b.build_atlas_commands(dq.queue());
        }

        auto extent = ext.extent_2d();
        {
          one_time_submitter ots{cb};
          render_passer rp{cb, frm->framebuffer(), &ext};
          vee::cmd_set_scissor(cb, extent);
          vee::cmd_set_viewport(cb, extent);

          ppl.build_commands(cb);

          for (auto &b : m_batches) {
            int n = b.build_commands(cb);
            if (n > 0)
              ps.run(cb, n);
          }
        }

        inf.submit(dq.queue(), cb);

        vee::queue_present({
            .queue = dq.queue(),
            .swapchain = ext.swapchain(),
            .wait_semaphore = inf.render_finished_sema(),
            .image_index = idx,
        });
      });
    }
  }
};
} // namespace quack
