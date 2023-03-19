module quack;
import :v_per_device;
import :v_per_extent;
import :v_per_frame;
import :v_per_inflight;
import :v_pipeline;
import hai;
import casein;
import vee;

namespace quack {
class pimpl {
  hai::uptr<per_device> m_dev{};
  hai::uptr<per_extent> m_ext{};
  hai::uptr<inflight_pair> m_infs{};
  hai::holder<hai::uptr<per_frame>[]> m_frms{};
  hai::uptr<pipeline> m_ppl{};
  unsigned m_max_quads;

public:
  explicit pimpl(unsigned max_quad) : m_max_quads{max_quad} {}

  void setup(casein::native_handle_t nptr) {
    m_dev = hai::uptr<per_device>::make(nptr);
    m_ext = hai::uptr<per_extent>::make(&*m_dev);
    m_infs = hai::uptr<inflight_pair>::make(&*m_dev);
    m_ppl = hai::uptr<pipeline>::make(&*m_dev, &*m_ext, m_max_quads);

    auto imgs = vee::get_swapchain_images(m_ext->swapchain());
    m_frms = decltype(m_frms)::make(imgs.size());
    for (auto i = 0; i < imgs.size(); i++) {
      auto img = (imgs.data())[i];
      (*m_frms)[i] = hai::uptr<per_frame>::make(&*m_dev, &*m_ext, img);
    }
  }

  [[nodiscard]] auto &ppl() noexcept { return *m_ppl; }
};

renderer::renderer(const params &p)
    : m_pimpl{hai::uptr<pimpl>::make(p.max_quads)} {}
renderer::~renderer() = default;

void renderer::fill_colour(const filler<colour> &g) {
  m_pimpl->ppl().map_instances_colour(g);
}
void renderer::fill_pos(const filler<pos> &g) {
  m_pimpl->ppl().map_instances_pos(g);
}
void renderer::repaint(unsigned i_count) {}

void renderer::setup(casein::native_handle_t nptr) {
  vee::initialise();
  m_pimpl->setup(nptr);
}

void renderer::quit() {
  vee::device_wait_idle();
  m_pimpl = {};
}
} // namespace quack
