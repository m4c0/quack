export module quack:pipeline;
import :per_extent;
import :pipeline_stuff;
import vee;

namespace quack {
class pipeline {
  vee::gr_pipeline gp;

public:
  explicit pipeline(const per_extent *ext, const pipeline_stuff *ps)
      : gp{ps->create_pipeline(ext)} {}

  void build_commands(vee::command_buffer cb) const {
    vee::cmd_bind_gr_pipeline(cb, *gp);
  }
};
} // namespace quack
