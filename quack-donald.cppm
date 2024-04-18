export module quack:donald;
import :ibatch;
import :pipeline_stuff;
import voo;

namespace quack {
/// Single-batch single-atlas render thread
export class donald : public voo::casein_thread {
  friend class donald_ibt;

protected:
  virtual const char *app_name() const noexcept = 0;
  virtual unsigned max_quads() const noexcept = 0;
  virtual void update_data(mapped_buffers p) = 0;

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

  donald_ibt ib{dq.queue(), ps, max_quads(), this};

  while (!interrupted()) {
  }
}
