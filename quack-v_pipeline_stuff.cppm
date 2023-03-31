export module quack:v_pipeline_stuff;
import :objects;
import :v_bbuffer;
import :v_per_device;
import :v_per_extent;
import vee;

namespace quack {
struct pcs {
  pos grid_pos{};
  pos grid_size{};
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
  pipeline_stuff(const per_device *d, unsigned max_quads)
      : dev{d}, instance_pos{dev, max_quads}, instance_colour{dev, max_quads},
        instance_uv{dev, max_quads} {
    map_vertices();
  }

  void resize(const params &p, float aspect) {
    float gw = p.grid_w / 2.0;
    float gh = p.grid_h / 2.0;
    float grid_aspect = gw / gh;
    auto grid_pos = pos{gw, gh};
    auto grid_size =
        grid_aspect < aspect ? pos{aspect * gh, gh} : pos{gw, gw / aspect};
    pc = {grid_pos, grid_size};
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
} // namespace quack