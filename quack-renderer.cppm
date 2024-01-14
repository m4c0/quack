export module quack:renderer;
import :ibatch;
import :per_device;
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
  const per_device *m_dev;
  const pipeline_stuff *m_ps;

public:
  [[nodiscard]] auto *allocate_batch(unsigned max_quads) {
    auto lck = wait_init();
    auto pl = m_ps->pipeline_layout();
    auto ds = m_ps->allocate_descriptor_set();
    auto res = instance_batch{m_dev, pl, ds, max_quads};
    m_batches.push_back(traits::move(res));
    return &m_batches[m_batches.size() - 1];
  }

  void run() override {
    per_device dev{native_ptr()};
    inflight_pair ip{};
    pipeline_stuff ps{&dev, max_batches};

    m_dev = &dev;
    m_ps = &ps;
    release_init_lock();

    while (!interrupted()) {
      per_extent ext{&dev};
      pipeline ppl{&ext, &ps};
      frames frms{&dev, &ext};

      extent_loop([&] {
        auto &inf = ip.flip();
        auto idx = vee::acquire_next_image(ext.swapchain(),
                                           inf.image_available_sema());
        auto &frm = frms[idx];
        auto cb = frm->command_buffer();

        for (auto &b : m_batches) {
          b.build_atlas_commands(dev.queue());
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

        inf.submit(&dev, cb);

        vee::queue_present({
            .queue = dev.queue(),
            .swapchain = ext.swapchain(),
            .wait_semaphore = inf.render_finished_sema(),
            .image_index = idx,
        });
      });
    }
  }
};
} // namespace quack
