export module quack:ibatch;
import :objects;
import dotz;
import jute;
import silog;
import traits;
import vee;
import voo;

namespace quack {
export class instance_batch {
  voo::h2l_buffer m_buffer;

public:
  instance_batch() = default;
  instance_batch(vee::physical_device pd, vee::pipeline_layout::type pl,
                 unsigned max_quads)
      : m_buffer{pd, static_cast<unsigned>(max_quads * sizeof(instance))} {}

  void map(auto &&fn) noexcept {
    voo::mapmem m{m_buffer.host_memory()};
    fn(static_cast<instance *>(*m));
  }

  void setup_copy(vee::command_buffer cb) const { m_buffer.setup_copy(cb); }

  void build_commands(vee::command_buffer cb) const {
    vee::cmd_bind_vertex_buffers(cb, 1, m_buffer.local_buffer());
  }
};

export class instance_batch_thread : public voo::updater<instance_batch> {
  void (*m_fn)(instance *){};

protected:
  virtual void update_data(instance *a) { m_fn(a); }
  virtual void update_data(instance_batch *ib) override {
    ib->map([this](auto a) { update_data(a); });
  }

public:
  instance_batch_thread(voo::queue *q, instance_batch ib)
      : updater{q, traits::move(ib)} {}
  instance_batch_thread(voo::queue *q, instance_batch ib,
                        void (*fn)(instance *))
      : updater{q, traits::move(ib)}
      , m_fn{fn} {}

  using update_thread::run_once;
};
} // namespace quack
