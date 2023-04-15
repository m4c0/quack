export module quack:thread;
import :agg;
import :raii;
import :objects;
import casein;
import sith;
import vee;

namespace quack {
class level_2 {
  const per_inflight *m_inf;
  unsigned m_idx;
  const per_frame *m_frm;
  vee::command_buffer m_cb;
  one_time_submitter m_ots;

public:
  level_2(level_0 *l0, const level_1 *l1)
      : m_inf{&l0->flip()}, m_idx{vee::acquire_next_image(
                                l1->ext()->swapchain(),
                                m_inf->image_available_sema())},
        m_frm{&*l1->frm(m_idx)}, m_cb{m_frm->command_buffer()}, m_ots{m_cb} {}

  [[nodiscard]] constexpr const auto command_buffer() const noexcept {
    return m_cb;
  }
  [[nodiscard]] constexpr const auto *frame() const noexcept { return m_frm; }
};

class level_3 {
  render_passer m_rp;

public:
  level_3(const level_1 *l1, const level_2 *l2)
      : m_rp{l2->command_buffer(), l2->frame()->framebuffer(), l1->ext()} {}
};

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
