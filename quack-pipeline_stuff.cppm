export module quack:pipeline_stuff;
import :objects;
import :bbuffer;
import :ibatch;
import :per_device;
import :per_extent;
import missingno;
import vee;

namespace quack {
class pipeline_stuff {
  const per_device *dev;

  vee::descriptor_set_layout dsl =
      vee::create_descriptor_set_layout({vee::dsl_fragment_sampler()});

  vee::pipeline_layout pl = vee::create_pipeline_layout(
      {*dsl}, {vee::vert_frag_push_constant_range<upc>()});

  vee::shader_module vert =
      vee::create_shader_module_from_resource("quack.vert.spv");
  vee::shader_module frag =
      vee::create_shader_module_from_resource("quack.frag.spv");

  vee::descriptor_pool desc_pool =
      vee::create_descriptor_pool(1, {vee::combined_image_sampler()});
  vee::descriptor_set desc_set = vee::allocate_descriptor_set(*desc_pool, *dsl);
  vee::sampler smp = vee::create_sampler(vee::nearest_sampler);

  static constexpr const auto v_count = 6;

  bound_buffer<pos> vertices{bb_vertex{}, dev, v_count};
  instance_batch instances;

  pos m_mouse_pos{};

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
  pipeline_stuff(const per_device *d, unsigned max_quads)
      : dev{d}, instances{dev, max_quads} {
    map_vertices();
  }

  void resize(const params &p, float sw, float sh) {
    instances.resize(p, sw, sh);
  }

  mno::opt<unsigned> current_hover() {
    return instances.current_hover(m_mouse_pos);
  }
  void mouse_move(float x, float y) { m_mouse_pos = {x, y}; }

  void set_atlas(const vee::image_view &iv) {
    vee::update_descriptor_set(desc_set, 0, *iv, *smp);
  }

  void map_instances_pos(const filler<pos> &fn) {
    instances.positions().map(fn);
  }
  void map_instances_colour(const filler<colour> &fn) {
    instances.colours().map(fn);
  }
  void map_instances_uv(const filler<uv> &fn) { instances.uvs().map(fn); }

  void build_commands(vee::command_buffer cb, unsigned i_count) {
    instances.set_count(i_count);
    vee::cmd_bind_vertex_buffers(cb, 0, *vertices);
    vee::cmd_bind_vertex_buffers(cb, 1, *instances.positions());
    vee::cmd_bind_vertex_buffers(cb, 2, *instances.colours());
    vee::cmd_bind_vertex_buffers(cb, 3, *instances.uvs());
    vee::cmd_bind_descriptor_set(cb, *pl, 0, desc_set);
    vee::cmd_push_vert_frag_constants(cb, *pl, &instances.push_constants());
    vee::cmd_draw(cb, v_count, instances.count());
  }

  [[nodiscard]] auto create_pipeline(const per_extent *ext) const {
    return vee::create_graphics_pipeline(
        *pl, ext->render_pass(),
        {
            vee::pipeline_vert_stage(*vert, "main"),
            vee::pipeline_frag_stage(*frag, "main"),
        },
        {
            vee::vertex_input_bind(sizeof(pos)),
            vee::vertex_input_bind_per_instance(sizeof(pos)),
            vee::vertex_input_bind_per_instance(sizeof(colour)),
            vee::vertex_input_bind_per_instance(sizeof(uv)),
        },
        {
            vee::vertex_attribute_vec2(0, 0),
            vee::vertex_attribute_vec2(1, 0),
            vee::vertex_attribute_vec4(2, 0),
            vee::vertex_attribute_vec4(3, 0),
        });
  }
};
} // namespace quack
