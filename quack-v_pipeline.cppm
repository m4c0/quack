export module quack:v_pipeline;
import :objects;
import :v_bbuffer;
import :v_per_device;
import :v_per_extent;
import vee;

namespace quack {
class pcs {
  pos grid_pos;
  pos grid_size;

  pcs(float gw, float gh) : grid_pos{gw, gh}, grid_size{gw, gh} {}

public:
  pcs(const params &p) : pcs(p.grid_w / 2.0f, p.grid_h / 2.0f) {}
};
class pipeline_stuff {
  const per_device *dev;

  vee::descriptor_set_layout dsl =
      vee::create_descriptor_set_layout({vee::dsl_fragment_sampler()});

  vee::pipeline_layout pl = vee::create_pipeline_layout(
      {*dsl}, {
                  vee::vertex_push_constant_range<pcs>(),
              });

  vee::shader_module vert =
      vee::create_shader_module_from_resource("quack.vert.spv");
  vee::shader_module frag =
      vee::create_shader_module_from_resource("quack.frag.spv");

  vee::descriptor_pool desc_pool =
      vee::create_descriptor_pool(1, {vee::combined_image_sampler()});
  vee::descriptor_set desc_set = vee::allocate_descriptor_set(*desc_pool, *dsl);
  vee::sampler smp = vee::create_sampler(vee::nearest_sampler);

  static constexpr const auto v_count = 6;

  bound_buffer<pos> vertices{dev, v_count};
  bound_buffer<pos> instance_pos;
  bound_buffer<colour> instance_colour;
  bound_buffer<uv> instance_uv;
  pcs pc;

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
  pipeline_stuff(const per_device *d, const params &p)
      : dev{d}, instance_pos{dev, p.max_quads}, instance_colour{dev,
                                                                p.max_quads},
        instance_uv{dev, p.max_quads}, pc{p} {
    map_vertices();
  }

  void set_atlas(const vee::image_view &iv) {
    vee::update_descriptor_set(desc_set, 0, *iv, *smp);
  }

  void map_instances_pos(const filler<pos> &fn) { instance_pos.map(fn); }
  void map_instances_colour(const filler<colour> &fn) {
    instance_colour.map(fn);
  }
  void map_instances_uv(const filler<uv> &fn) { instance_uv.map(fn); }

  void build_commands(vee::command_buffer cb, unsigned i_count) const {
    vee::cmd_bind_vertex_buffers(cb, 0, *vertices);
    vee::cmd_bind_vertex_buffers(cb, 1, *instance_pos);
    vee::cmd_bind_vertex_buffers(cb, 2, *instance_colour);
    vee::cmd_bind_vertex_buffers(cb, 3, *instance_uv);
    vee::cmd_bind_descriptor_set(cb, *pl, 0, desc_set);
    vee::cmd_push_vertex_constants(cb, *pl, &pc);
    vee::cmd_draw(cb, v_count, i_count);
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
