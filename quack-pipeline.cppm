export module quack:pipeline;
import :per_extent;
import :pipeline_stuff;
import vee;

namespace quack {
class pipeline {
  const per_extent *m_ext;
  const pipeline_stuff *m_stuff;

  vee::gr_pipeline gp = m_stuff->create_pipeline(m_ext);

public:
  explicit pipeline(const per_extent *ext, const pipeline_stuff *ps)
      : m_ext{ext}, m_stuff{ps} {}

  void build_commands(vee::command_buffer cb, unsigned i_count) const {
    const auto extent = m_ext->extent_2d();

    vee::begin_cmd_buf_render_pass_continue(cb, m_ext->render_pass());
    vee::cmd_set_scissor(cb, extent);
    vee::cmd_set_viewport(cb, extent);
    vee::cmd_bind_gr_pipeline(cb, *gp);
    m_stuff->build_commands(cb, i_count);
    vee::end_cmd_buf(cb);
  }
};
} // namespace quack
