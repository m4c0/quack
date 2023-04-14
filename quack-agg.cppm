export module quack:agg;
import :objects;
import :per_device;
import :per_extent;
import :per_frame;
import :per_inflight;
import :pipeline;
import :pipeline_stuff;
import :stage;
import casein;
import missingno;

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
} // namespace quack
