module quack;
import :per_device;
import :per_extent;
import :per_frame;
import :per_inflight;
import :pipeline;
import :pipeline_stuff;
import :stage;
import :thread;
import hai;
import casein;
import missingno;
import vee;

namespace quack {
class vpimpl : public pimpl {
  hai::uptr<per_device> m_dev{};
  hai::uptr<per_extent> m_ext{};
  hai::uptr<inflight_pair> m_infs{};
  hai::uptr<frames> m_frms{};
  hai::uptr<stage_image> m_stg{};
  hai::uptr<pipeline_stuff> m_ps{};
  hai::uptr<pipeline> m_ppl{};
  params m_p;

public:
  explicit vpimpl(const params &p) : m_p{p} {}
  virtual ~vpimpl() { vee::device_wait_idle(); }

  void setup(casein::native_handle_t nptr) override {
    vee::initialise();
    m_dev = hai::uptr<per_device>::make(nptr);
    m_infs = hai::uptr<inflight_pair>::make(&*m_dev);
    m_stg = hai::uptr<stage_image>::make(&*m_dev);
    m_ps = hai::uptr<pipeline_stuff>::make(&*m_dev, m_p.max_quads);
    resize();
  }
  void resize() {
    vee::device_wait_idle();
    m_ext = hai::uptr<per_extent>::make(&*m_dev);
    m_ppl = hai::uptr<pipeline>::make(&*m_ext, &*m_ps);
    m_frms = hai::uptr<frames>::make(&*m_dev, &*m_ext);
  }
  void resize(unsigned w, unsigned h, float scale) override {
    m_ps->resize(m_p, w, h);
    resize();
  }

  mno::opt<unsigned> current_hover() override { return m_ps->current_hover(); }

  void mouse_move(unsigned x, unsigned y) override { m_ps->mouse_move(x, y); }

  void repaint(unsigned i_count) override {
    try {
      auto &inf = m_infs->flip();

      auto idx = inf.wait_and_takeoff(&*m_ext);

      m_ppl->build_commands(inf.command_buffer(), i_count);

      const auto exec_secondary = [&inf](auto cb) {
        vee::cmd_execute_command(cb, inf.command_buffer());
      };
      const auto prepare_stage = [stg = &*m_stg](auto cb) {
        stg->build_commands(cb);
      };

      inf.submit(&*m_dev, (*m_frms)[idx]->one_time_submit(prepare_stage,
                                                          exec_secondary));
      vee::queue_present({
          .queue = m_dev->queue(),
          .swapchain = m_ext->swapchain(),
          .wait_semaphore = inf.render_finished_sema(),
          .image_index = idx,
      });
    } catch (vee::out_of_date_error) {
      resize();
    }
  }

  void fill_colour(const filler<colour> &g) override {
    m_ps->map_instances_colour(g);
  }
  void fill_pos(const filler<pos> &g) override { m_ps->map_instances_pos(g); }
  void fill_uv(const filler<uv> &g) override { m_ps->map_instances_uv(g); }

  void load_atlas(unsigned w, unsigned h, const filler<u8_rgba> &g) override {
    if (m_stg->resize_image(w, h))
      m_ps->set_atlas(m_stg->image_view());

    m_stg->load_image(g);
  }
};

renderer::renderer(const params &p)
    : m_pimpl{hai::uptr<pimpl>(new vpimpl(p))} {}
} // namespace quack
