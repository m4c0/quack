export module quack:thread;
import :agg;
import :raii;
import :objects;
import casein;
import sith;
import vee;

namespace quack {
class thread : public sith::thread {
  casein::native_handle_t m_nptr;
  unsigned m_max_quads;

  void run() override {
    vee::initialise();
    level_0 l0{m_nptr};

    while (!interrupted()) {
      try {
        vee::device_wait_idle();
        level_1 l1{&l0};
        const auto extent = l1.ext()->extent_2d();

        while (!interrupted()) {

          level_2 l2{&l0, &l1};
          const auto cb = l2.command_buffer();
          l0.stg()->build_commands(cb);

          level_3 l3{&l1, &l2};
          vee::cmd_set_scissor(cb, extent);
          vee::cmd_set_viewport(cb, extent);

          l1.ppl()->build_commands(cb);
          l0.ps()->build_commands(cb);
        }
      } catch (vee::out_of_date_error) {
        vee::device_wait_idle();
      }
    }
  }

public:
  explicit thread(casein::native_handle_t nptr, unsigned max_batches,
                  unsigned max_quads)
      : sith::thread{false}, m_nptr{nptr}, m_max_quads{max_quads} {}
};
} // namespace quack
