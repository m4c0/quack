export module quack:thread;
import :agg;
import :raii;
import :objects;
import casein;
import hai;
import traits;
import sith;
import vee;

namespace quack {
class thread : public sith::thread {
  hai::holder<hai::uptr<instance_batch>[]> m_batches{};
  casein::native_handle_t m_nptr;
  unsigned m_batch_count;
  volatile bool m_reset_l1{false};
  level_0 m_l0{m_nptr};

  void run() override {
    while (!interrupted()) {
      m_reset_l1 = true;

      try {
        vee::device_wait_idle();
        level_1 l1{&m_l0};
        const auto extent = l1.ext()->extent_2d();

        while (!interrupted() && !m_reset_l1) {
          level_2 l2{&m_l0, &l1};
          const auto cb = l2.command_buffer();
          m_l0.stg()->build_commands(cb);

          level_3 l3{&l1, &l2};
          vee::cmd_set_scissor(cb, extent);
          vee::cmd_set_viewport(cb, extent);

          l1.ppl()->build_commands(cb);
          m_l0.ps()->build_commands(cb);

          for (auto i = 0; i < m_batch_count; i++) {
            (*m_batches)[i]->build_commands(cb);
          }
        }
      } catch (vee::out_of_date_error) {
        vee::device_wait_idle();
      }
    }
  }

public:
  explicit thread(casein::native_handle_t nptr, unsigned max_batches)
      : sith::thread{false}, m_batches{decltype(m_batches)::make(max_batches)},
        m_nptr{nptr} {}

  // Non-thread safe. Must be called on main thread before start
  [[nodiscard]] instance_batch *allocate(unsigned max_quads) noexcept {
    auto &res = (*m_batches)[m_batch_count++];
    res = hai::uptr<instance_batch>::make(
        m_l0.dev(), m_l0.ps()->pipeline_layout(), max_quads);
    return &*res;
  }

  void load_atlas(unsigned w, unsigned h, auto &&fn) {
    if (m_l0.stg()->resize_image(w, h))
      m_l0.ps()->set_atlas(m_l0.stg()->image_view());

    m_l0.stg()->load_image(traits::move(fn));
  }

  void reset_l1() noexcept { m_reset_l1 = true; }
};
} // namespace quack
