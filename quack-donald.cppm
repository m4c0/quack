export module quack:donald;
import :ibatch;
import :pipeline_stuff;
import :upc;
import hai;
import sith;
import vee;
import voo;

namespace quack {
/// Single-batch single-atlas render thread
export class donald : public voo::casein_thread {
  friend class donald_ibt;

protected:
  virtual const char *app_name() const noexcept = 0;
  virtual unsigned max_quads() const noexcept = 0;
  virtual void update_data(mapped_buffers p) = 0;
  virtual quack::upc push_constants() const noexcept = 0;

  virtual hai::uptr<voo::updater<voo::h2l_image>>
  create_atlas(const voo::device_and_queue *) = 0;

  virtual unsigned quad_count() const noexcept { return max_quads(); }
  virtual vee::sampler create_sampler() {
    return vee::create_sampler(vee::nearest_sampler);
  }

public:
  void run() override;
};

class donald_ibt : public instance_batch_thread {
  donald *m_d;

  void update_data(quack::mapped_buffers p) override { m_d->update_data(p); }

public:
  donald_ibt(voo::queue *q, quack::pipeline_stuff &ps, unsigned max_quads,
             donald *d)
      : instance_batch_thread{q, ps.create_batch(max_quads)}
      , m_d{d} {}
};
} // namespace quack

void quack::donald::run() {
  voo::device_and_queue dq{app_name(), native_ptr()};
  pipeline_stuff ps{dq, 1};

  auto atlas = create_atlas(&dq);
  atlas->run_once();

  donald_ibt ib{dq.queue(), ps, max_quads(), this};

  while (!interrupted()) {
    voo::swapchain_and_stuff sw{dq};
    sith::run_guard ibr{&ib};

    auto smp = create_sampler();
    auto dset = ps.allocate_descriptor_set(atlas->data().iv(), *smp);

    extent_loop(dq.queue(), sw, [&] {
      auto upc = quack::adjust_aspect(push_constants(), sw.aspect());
      sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
        auto scb = sw.cmd_render_pass(pcb);
        vee::cmd_set_viewport(*scb, sw.extent());
        vee::cmd_set_scissor(*scb, sw.extent());
        ib.data().build_commands(*pcb);
        ps.cmd_bind_descriptor_set(*scb, dset);
        ps.cmd_push_vert_frag_constants(*scb, upc);
        ps.run(*scb, quad_count());
      });
    });
  }
}
