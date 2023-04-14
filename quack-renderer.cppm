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
  hai::uptr<instance_batch> m_batch{};
  params m_p;
  pos m_mouse_pos{};

  [[nodiscard]] auto build_primary_cmd_buf(const per_frame &frm) {
    one_time_submitter ots{frm.command_buffer()};
    ots([this](auto cb) { m_l0->stg()->build_commands(cb); });
    ots([&, this](auto cb) {
      render_passer rp{cb, frm.framebuffer(), m_l1->ext()};
      build_render_pass(cb);
    });
    return frm.command_buffer();
  }
  void build_render_pass(vee::command_buffer scb) {
    const auto extent = m_l1->ext()->extent_2d();
    vee::cmd_set_scissor(scb, extent);
    vee::cmd_set_viewport(scb, extent);

    m_l1->ppl()->build_commands(scb);
    m_l0->ps()->build_commands(scb);
    m_batch->build_commands(scb);
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
    m_l0 = hai::uptr<level_0>::make(nptr);
    m_batch = hai::uptr<instance_batch>::make(
        m_l0->dev(), m_l0->ps()->pipeline_layout(), m_p.max_quads);
    resize();
  }

  void resize(unsigned w, unsigned h, float scale) {
    m_batch->resize(m_p, w, h);
    resize();
  }

  void repaint(unsigned i_count) {
    m_batch->set_count(i_count);

    try {
      auto &inf = m_l0->flip();

      auto idx = vee::acquire_next_image(m_l1->ext()->swapchain(),
                                         inf.image_available_sema());

      auto cb = build_primary_cmd_buf(*m_l1->frm(idx));
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
    m_batch = {};
    m_l1 = {};
    m_l0 = {};
  }

  template <typename Fn> void fill_pos(Fn &&fn) {
    m_batch->positions().map(traits::move(fn));
  }
  template <typename Fn> void fill_colour(Fn &&fn) {
    m_batch->colours().map(traits::move(fn));
  }
  template <typename Fn> void fill_uv(Fn &&fn) {
    m_batch->uvs().map(traits::move(fn));
  }
  template <typename Fn> void load_atlas(unsigned w, unsigned h, Fn &&fn) {
    if (m_l0->stg()->resize_image(w, h))
      m_l0->ps()->set_atlas(m_l0->stg()->image_view());

    m_l0->stg()->load_image(traits::move(fn));
  }

  void mouse_move(float x, float y) { m_mouse_pos = {x, y}; }
  [[nodiscard]] mno::opt<unsigned> current_hover() {
    return m_batch->current_hover(m_mouse_pos);
  }

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
