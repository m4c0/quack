module quack;
import :v_per_device;
import :v_per_extent;
import :v_per_frame;
import :v_per_inflight;
import :v_pipeline;
import :v_stage;
import hai;
import casein;
import vee;

namespace quack {
class pimpl {
  hai::uptr<per_device> m_dev{};
  hai::uptr<per_extent> m_ext{};
  hai::uptr<inflight_pair> m_infs{};
  hai::holder<hai::uptr<per_frame>[]> m_frms{};
  hai::uptr<stage_image> m_stg{};
  hai::uptr<pipeline> m_ppl{};
  params m_p;

public:
  explicit pimpl(const params &p) : m_p{p} {}

  void setup(casein::native_handle_t nptr) {
    m_dev = hai::uptr<per_device>::make(nptr);
    setup();
  }
  void setup() {
    m_ext = hai::uptr<per_extent>::make(&*m_dev);
    m_infs = hai::uptr<inflight_pair>::make(&*m_dev);
    m_stg = hai::uptr<stage_image>::make(&*m_dev);
    m_ppl = hai::uptr<pipeline>::make(&*m_dev, &*m_ext, m_p);

    m_ppl->set_atlas(m_stg->image_view());

    auto imgs = vee::get_swapchain_images(m_ext->swapchain());
    m_frms = decltype(m_frms)::make(imgs.size());
    for (auto i = 0; i < imgs.size(); i++) {
      auto img = (imgs.data())[i];
      (*m_frms)[i] = hai::uptr<per_frame>::make(&*m_dev, &*m_ext, img);
    }
  }

  void paint(unsigned i_count) {
    try {
      auto &inf = m_infs->flip();

      auto idx = inf.wait_and_takeoff(&*m_ext);

      m_ppl->build_commands(inf.command_buffer(), i_count);

      inf.submit(&*m_dev, (*m_frms)[idx]->one_time_submit([&inf](auto cb) {
        vee::cmd_execute_command(cb, inf.command_buffer());
      }));
      vee::queue_present({
          .queue = m_dev->queue(),
          .swapchain = m_ext->swapchain(),
          .wait_semaphore = inf.render_finished_sema(),
          .image_index = idx,
      });
    } catch (vee::out_of_date_error) {
      vee::device_wait_idle();
      setup();
    }
  }

  [[nodiscard]] auto &ppl() noexcept { return *m_ppl; }
};

renderer::renderer(const params &p) : m_pimpl{hai::uptr<pimpl>::make(p)} {}
renderer::~renderer() = default;

void renderer::_fill_colour(const filler<colour> &g) {
  m_pimpl->ppl().map_instances_colour(g);
}
void renderer::_fill_pos(const filler<pos> &g) {
  m_pimpl->ppl().map_instances_pos(g);
}
void renderer::_fill_uv(const filler<uv> &g) {
  m_pimpl->ppl().map_instances_uv(g);
}
void renderer::repaint(unsigned i_count) { m_pimpl->paint(i_count); }

void renderer::setup(casein::native_handle_t nptr) {
  vee::initialise();
  m_pimpl->setup(nptr);
}

void renderer::quit() {
  vee::device_wait_idle();
  m_pimpl = {};
}
} // namespace quack
