export module quack:agg;
import :objects;
import :per_device;
import :per_extent;
import :per_frame;
import :per_inflight;
import :pipeline;
import :pipeline_stuff;
import :raii;
import :stage;
import casein;
import missingno;
import vee;

namespace quack {
class level_0 {
  per_device m_dev;
  inflight_pair m_inf;
  stage_image m_stg;
  pipeline_stuff m_ps;

public:
  level_0(casein::native_handle_t nptr)
      : m_dev{nptr}, m_inf{}, m_stg{&m_dev}, m_ps{&m_dev} {}

  [[nodiscard]] constexpr const auto *dev() const noexcept { return &m_dev; }
  [[nodiscard]] constexpr const auto *ps() const noexcept { return &m_ps; }
  [[nodiscard]] constexpr auto *ps() noexcept { return &m_ps; }
  [[nodiscard]] constexpr auto *stg() noexcept { return &m_stg; }

  [[nodiscard]] auto &flip() { return m_inf.flip(); }
};

class level_1 {
  per_extent m_ext;
  pipeline m_ppl;
  frames m_frms;

public:
  level_1(const level_0 *l0)
      : m_ext{l0->dev()}, m_ppl{&m_ext, l0->ps()}, m_frms{l0->dev(), &m_ext} {}

  [[nodiscard]] constexpr const auto *ext() const noexcept { return &m_ext; }
  [[nodiscard]] constexpr const auto *ppl() const noexcept { return &m_ppl; }

  [[nodiscard]] constexpr const auto &frm(unsigned i) const noexcept {
    return m_frms[i];
  }
};

class level_2 {
  const level_0 *m_l0;
  const level_1 *m_l1;
  const per_inflight *m_inf;
  unsigned m_idx;
  const per_frame *m_frm;
  vee::command_buffer m_cb;
  one_time_submitter m_ots;

public:
  level_2(level_0 *l0, const level_1 *l1)
      : m_l0{l0}, m_l1{l1}, m_inf{&l0->flip()},
        m_idx{vee::acquire_next_image(l1->ext()->swapchain(),
                                      m_inf->image_available_sema())},
        m_frm{&*l1->frm(m_idx)}, m_cb{m_frm->command_buffer()}, m_ots{m_cb} {}

  ~level_2() {
    m_inf->submit(m_l0->dev(), m_cb);

    vee::queue_present({
        .queue = m_l0->dev()->queue(),
        .swapchain = m_l1->ext()->swapchain(),
        .wait_semaphore = m_inf->render_finished_sema(),
        .image_index = m_idx,
    });
  }

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
} // namespace quack
