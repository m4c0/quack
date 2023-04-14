export module quack:renderer;
import :agg;
import :raii;
import :objects;
import casein;
import hai;
import missingno;
import traits;
import vee;

export namespace quack {
class renderer {
  hai::uptr<level_0> m_l0{};
  hai::uptr<level_1> m_l1{};
  params m_p;

  template <typename Tp, typename Fn> class s : public filler<Tp> {
    Fn m;

  public:
    constexpr s(Fn &&fn) : m{fn} {}
    void operator()(Tp *c) const noexcept { m(c); }
  };

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
  void build_secondary_cmd_buf(vee::command_buffer scb, unsigned i_count) {
    render_pass_continuer rpc{scb, m_l1->ext()};

    const auto extent = m_l1->ext()->extent_2d();
    vee::cmd_set_scissor(scb, extent);
    vee::cmd_set_viewport(scb, extent);

    m_l1->ppl()->build_commands(scb);
    m_l0->ps()->build_commands(scb, i_count);
  }

  void resize() {
    vee::device_wait_idle();
    m_l1 = hai::uptr<level_1>::make(&*m_l0);
  }

public:
  explicit renderer(const params &p) : m_p{p} {}
  ~renderer() { vee::device_wait_idle(); }

  void setup(casein::native_handle_t nptr) {
    vee::initialise();
    m_l0 = hai::uptr<level_0>::make(nptr, m_p.max_quads);
    resize();
  }

  void resize(unsigned w, unsigned h, float scale) {
    m_l0->ps()->resize(m_p, w, h);
    resize();
  }

  void repaint(unsigned i_count) {
    try {
      auto &inf = m_l0->flip();

      auto idx = vee::acquire_next_image(m_l1->ext()->swapchain(),
                                         inf.image_available_sema());

      build_secondary_cmd_buf(inf.command_buffer(), i_count);

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

  void quit() {
    vee::device_wait_idle();
    m_l1 = {};
    m_l0 = {};
  }

  template <typename Fn> void fill_pos(Fn &&fn) {
    m_l0->ps()->map_instances_pos(traits::move(fn));
  }
  template <typename Fn> void fill_colour(Fn &&fn) {
    m_l0->ps()->map_instances_colour(traits::move(fn));
  }
  template <typename Fn> void fill_uv(Fn &&fn) {
    m_l0->ps()->map_instances_uv(traits::move(fn));
  }
  template <typename Fn> void load_atlas(unsigned w, unsigned h, Fn &&fn) {
    if (m_l0->stg()->resize_image(w, h))
      m_l0->ps()->set_atlas(m_l0->stg()->image_view());

    m_l0->stg()->load_image(traits::move(fn));
  }

  [[nodiscard]] mno::opt<unsigned> current_hover() {
    return m_l0->ps()->current_hover();
  }

  void mouse_move(unsigned x, unsigned y) { m_l0->ps()->mouse_move(x, y); }

  void process_event(const casein::event &e) {
    switch (e.type()) {
    case casein::CREATE_WINDOW:
      setup(*e.as<casein::events::create_window>());
      break;
    case casein::RESIZE_WINDOW: {
      const auto &[w, h, scale, live] = *e.as<casein::events::resize_window>();
      resize(w, h, scale);
      break;
    }
    case casein::MOUSE_DOWN: {
      const auto &[x, y, btn] = *e.as<casein::events::mouse_down>();
      mouse_move(x, y);
      break;
    }
    case casein::MOUSE_UP: {
      const auto &[x, y, btn] = *e.as<casein::events::mouse_up>();
      mouse_move(x, y);
      break;
    }
    case casein::MOUSE_MOVE: {
      const auto &[x, y] = *e.as<casein::events::mouse_move>();
      mouse_move(x, y);
      break;
    }
    case casein::QUIT:
      quit();
      break;
    default:
      break;
    }
  }
};
} // namespace quack
