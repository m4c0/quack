export module quack:thread;
import :agg;
import :ibatch;
import :raii;
import :objects;
import casein;
import hai;
import traits;
import sith;
import vee;

namespace quack {
class thread : public sith::thread {
  volatile unsigned m_batch_count{};
  volatile bool m_reset_l1{false};
  level_0 m_l0;
  hai::holder<hai::uptr<instance_batch>[]> m_batches;

  void run() override {
    while (!interrupted()) {
      try {
        run_l1();
      } catch (vee::out_of_date_error) {
        // Nothing to be done here. This point is too late for a "wait idle"
        // since the scope already died
      }
    }
  }

  void run_l1() {
    level_1 l1{&m_l0};
    const auto extent = l1.ext()->extent_2d();

    m_reset_l1 = false;
    while (!interrupted() && !m_reset_l1) {
      level_2 l2{&m_l0, &l1};
      const auto cb = l2.command_buffer();

      for (auto i = 0; i < m_batch_count; i++) {
        (*m_batches)[i]->build_atlas_commands(m_l0.dev()->queue());
      }

      one_time_submitter ots{cb};
      level_3 l3{&l1, &l2};

      vee::cmd_set_scissor(cb, extent);
      vee::cmd_set_viewport(cb, extent);

      l1.ppl()->build_commands(cb);
      m_l0.ps()->build_commands(cb);

      for (auto i = 0; i < m_batch_count; i++) {
        (*m_batches)[i]->build_commands(cb);
      }
    }

    vee::device_wait_idle();
  }

public:
  explicit thread(casein::native_handle_t nptr, unsigned max_batches)
      : sith::thread{}, m_l0{nptr, max_batches},
        m_batches{decltype(m_batches)::make(max_batches)} {}
  ~thread() {
    stop();
    vee::device_wait_idle();
  }

  [[nodiscard]] instance_batch *allocate(unsigned max_quads) noexcept {
    auto &res = (*m_batches)[m_batch_count];
    res = hai::uptr<instance_batch>::make(
        m_l0.dev(), m_l0.ps()->pipeline_layout(),
        m_l0.ps()->allocate_descriptor_set(), max_quads);
    m_batch_count = m_batch_count + 1;
    return &*res;
  }
  void reset_l1() noexcept { m_reset_l1 = true; }
};
} // namespace quack
