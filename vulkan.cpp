module quack;
import :agg;
import :raii;
import :thread;
import hai;
import casein;
import missingno;
import vee;

namespace quack {
class vpimpl : public pimpl {
  hai::uptr<level_0> m_l0{};
  hai::uptr<level_1> m_l1{};
  params m_p;

public:
  explicit vpimpl(const params &p) : m_p{p} {}
  virtual ~vpimpl() { vee::device_wait_idle(); }

  void setup(casein::native_handle_t nptr) override {
    vee::initialise();
    m_l0 = hai::uptr<level_0>::make(nptr, m_p.max_quads);
    resize();
  }
  void resize() {
    vee::device_wait_idle();
    m_l1 = hai::uptr<level_1>::make(&*m_l0);
  }
  void resize(unsigned w, unsigned h, float scale) override {
    m_l0->ps()->resize(m_p, w, h);
    resize();
  }

  mno::opt<unsigned> current_hover() override {
    return m_l0->ps()->current_hover();
  }
  void mouse_move(unsigned x, unsigned y) override {
    m_l0->ps()->mouse_move(x, y);
  }

  [[nodiscard]] auto build_primary_cmd_buf(const per_frame &frm,
                                           const per_inflight &inf) {
    one_time_submitter ots{frm.command_buffer()};
    ots([this](auto cb) { m_l0->stg()->build_commands(cb); });
    ots([&, this](auto cb) {
      render_passer rp{cb, frm.framebuffer(), m_l1->ext()};
      rp.execute(inf.command_buffer());
    });
    return frm.command_buffer();
  }
  void repaint(unsigned i_count) override {
    try {
      auto &inf = m_l0->flip();

      auto idx = vee::acquire_next_image(m_l1->ext()->swapchain(),
                                         inf.image_available_sema());

      m_l1->ppl()->build_commands(inf.command_buffer(), i_count);

      auto cb = build_primary_cmd_buf(*m_l1->frm(idx), inf);
      inf.submit(m_l0->dev(), cb);

      vee::queue_present({
          .queue = m_l0->dev()->queue(),
          .swapchain = m_l1->ext()->swapchain(),
          .wait_semaphore = inf.render_finished_sema(),
          .image_index = idx,
      });
    } catch (vee::out_of_date_error) {
      resize();
    }
  }

  void fill_colour(const filler<colour> &g) override {
    m_l0->ps()->map_instances_colour(g);
  }
  void fill_pos(const filler<pos> &g) override {
    m_l0->ps()->map_instances_pos(g);
  }
  void fill_uv(const filler<uv> &g) override {
    m_l0->ps()->map_instances_uv(g);
  }

  void load_atlas(unsigned w, unsigned h, const filler<u8_rgba> &g) override {
    if (m_l0->stg()->resize_image(w, h))
      m_l0->ps()->set_atlas(m_l0->stg()->image_view());

    m_l0->stg()->load_image(g);
  }
};

renderer::renderer(const params &p)
    : m_pimpl{hai::uptr<pimpl>(new vpimpl(p))} {}
} // namespace quack
