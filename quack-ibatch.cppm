export module quack:ibatch;
import :objects;
import dotz;
import jute;
import silog;
import traits;
import vee;
import voo;

namespace quack {
  export class instance_batch_thread : public voo::updater<voo::h2l_buffer> {
    static constexpr const unsigned instance_size = sizeof(instance);

    void (*m_fn)(instance *) {};

  protected:
    virtual void update_data(instance * a) { m_fn(a); }
    virtual void update_data(voo::h2l_buffer * buf) override {
      voo::mapmem m { buf->host_memory() };
      update_data(static_cast<instance *>(*m));
    }

  public:
    instance_batch_thread(voo::device_and_queue * dq, unsigned max_quads, void (*fn)(instance *))
        : updater { dq->queue(), voo::h2l_buffer { dq->physical_device(), max_quads * instance_size } }
        , m_fn { fn } {}

    using update_thread::run_once;
  };
} // namespace quack
