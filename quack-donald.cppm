export module quack:donald;
import :ibatch;
import :pipeline_stuff;
import :upc;
import hai;
import vee;
import voo;

namespace quack {
/// Single-batch single-atlas render thread
export class donald : public voo::casein_thread {
  friend class donald_ibt;
  voo::updater<voo::h2l_image> *m_atlas;
  instance_batch_thread *m_batch;

protected:
  using atlas = hai::uptr<voo::updater<voo::h2l_image>>;

  virtual const char *app_name() const noexcept = 0;
  virtual unsigned max_quads() const noexcept = 0;
  virtual void update_data(mapped_buffers p) = 0;
  virtual quack::upc push_constants() const noexcept = 0;

  virtual atlas create_atlas(voo::device_and_queue *) = 0;

  virtual unsigned quad_count() const noexcept { return max_quads(); }
  virtual vee::sampler create_sampler() {
    return vee::create_sampler(vee::nearest_sampler);
  }

public:
  void run() override;

  void refresh_atlas() {
    wait_init();
    m_atlas->run_once();
  }
  void refresh_batch() {
    wait_init();
    m_batch->run_once();
  }
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
