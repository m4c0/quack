export module quack:raii;
import :per_extent;
import vee;

namespace quack {
class one_time_submitter {
  vee::command_buffer m_cb;

public:
  explicit one_time_submitter(vee::command_buffer cb) : m_cb{cb} {
    vee::begin_cmd_buf_one_time_submit(cb);
  }
  ~one_time_submitter() { vee::end_cmd_buf(m_cb); }

  void operator()(auto fn) { fn(m_cb); }

  one_time_submitter(const one_time_submitter &) = delete;
  one_time_submitter(one_time_submitter &&) = delete;
  one_time_submitter &operator=(const one_time_submitter &) = delete;
  one_time_submitter &operator=(one_time_submitter &&) = delete;
};
class render_passer {
  vee::command_buffer m_cb;

public:
  explicit render_passer(vee::command_buffer cb, const vee::framebuffer &fb,
                         const per_extent *ext)
      : m_cb{cb} {
    vee::cmd_begin_render_pass({
        .command_buffer = cb,
        .render_pass = ext->render_pass(),
        .framebuffer = *fb,
        .extent = ext->extent_2d(),
        .clear_color = {0.05, 0.1f, 0.15, 1.0f},
    });
  }
  ~render_passer() { vee::cmd_end_render_pass(m_cb); }

  void execute(vee::command_buffer sec_cmd_buf) const {
    vee::cmd_execute_command(m_cb, sec_cmd_buf);
  }

  render_passer(const render_passer &) = delete;
  render_passer(render_passer &&) = delete;
  render_passer &operator=(const render_passer &) = delete;
  render_passer &operator=(render_passer &&) = delete;
};
class render_pass_continuer {
  vee::command_buffer m_cb;

public:
  explicit render_pass_continuer(vee::command_buffer cb, const per_extent *ext)
      : m_cb{cb} {
    vee::begin_cmd_buf_render_pass_continue(cb, ext->render_pass());
  }
  ~render_pass_continuer() { vee::end_cmd_buf(m_cb); }

  render_pass_continuer(const render_pass_continuer &) = delete;
  render_pass_continuer(render_pass_continuer &&) = delete;
  render_pass_continuer &operator=(const render_pass_continuer &) = delete;
  render_pass_continuer &operator=(render_pass_continuer &&) = delete;
};
} // namespace quack
