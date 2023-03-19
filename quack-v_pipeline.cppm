export module quack:v_pipeline;
import :objects;
import :v_bbuffer;
import :v_per_device;
import :v_per_extent;
import vee;

namespace quack {
struct pcs {
  pos grid_pos;
  pos grid_size;
};
class pipeline {
  const per_device *dev;
  const per_extent *ext;

  vee::pipeline_layout pl = vee::create_pipeline_layout({
      vee::vertex_push_constant_range<pcs>(),
  });

  vee::shader_module vert =
      vee::create_shader_module_from_resource("main.vert.spv");
  vee::shader_module frag =
      vee::create_shader_module_from_resource("main.frag.spv");
  vee::gr_pipeline gp = vee::create_graphics_pipeline(
      *pl, ext->render_pass(),
      {
          vee::pipeline_vert_stage(*vert, "main"),
          vee::pipeline_frag_stage(*frag, "main"),
      },
      {
          vee::vertex_input_bind(sizeof(pos)),
          vee::vertex_input_bind_per_instance(sizeof(pos)),
          vee::vertex_input_bind_per_instance(sizeof(colour)),
      },
      {
          vee::vertex_attribute_vec2(0, 0),
          vee::vertex_attribute_vec2(1, 0),
          vee::vertex_attribute_vec2(2, 0),
      });

  static constexpr const auto v_count = 6;

  bound_buffer<pos> vertices{dev, v_count};
  bound_buffer<pos> instance_pos;
  bound_buffer<colour> instance_colour;

  void map_vertices() {
    vertices.map([](auto vs) {
      vs[0] = {0, 0};
      vs[1] = {1, 1};
      vs[2] = {1, 0};

      vs[3] = {1, 1};
      vs[4] = {0, 0};
      vs[5] = {0, 1};
    });
  }

public:
  explicit pipeline(const per_device *d, const per_extent *e, unsigned max)
      : dev{d}, ext{e}, instance_pos{dev, max}, instance_colour{dev, max} {
    map_vertices();
  }

  void map_instances_pos(auto &&fn) { instance_pos.map(fn); }
  void map_instances_colour(auto &&fn) { instance_colour.map(fn); }

  void build_commands(vee::command_buffer cb, unsigned i_count) const {
    const auto extent = ext->extent_2d();
    constexpr const pcs pc{};

    vee::begin_cmd_buf_render_pass_continue(cb, ext->render_pass());
    vee::cmd_set_scissor(cb, extent);
    vee::cmd_set_viewport(cb, extent);
    vee::cmd_bind_gr_pipeline(cb, *gp);
    vee::cmd_bind_vertex_buffers(cb, 0, *vertices);
    vee::cmd_bind_vertex_buffers(cb, 1, *instance_pos);
    vee::cmd_bind_vertex_buffers(cb, 2, *instance_colour);
    vee::cmd_push_vertex_constants(cb, *pl, &pc);
    vee::cmd_draw(cb, v_count, i_count);
    vee::end_cmd_buf(cb);
  }
};
} // namespace quack
