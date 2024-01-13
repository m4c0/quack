export module quack:pipeline_stuff;
import :bbuffer;
import :objects;
import :per_device;
import :per_extent;
import missingno;
import traits;
import vee;
import voo;

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

  vee::descriptor_pool desc_pool;

  voo::one_quad m_quad{dev->physical_device()};

public:
  pipeline_stuff(const per_device *d, unsigned max_desc_sets)
      : dev{d},
        desc_pool{vee::create_descriptor_pool(
            max_desc_sets, {vee::combined_image_sampler(max_desc_sets)})} {}

  [[nodiscard]] constexpr const auto pipeline_layout() const noexcept {
    return *pl;
  }
  [[nodiscard]] constexpr auto allocate_descriptor_set() const noexcept {
    return vee::allocate_descriptor_set(*desc_pool, *dsl);
  }

  void run(vee::command_buffer cb, unsigned inst) const {
    m_quad.run(cb, 0, inst);
  }

  [[nodiscard]] auto create_pipeline(const per_extent *ext) const {
    return vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = ext->render_pass(),
        .depth_test = false,
        .shaders{
            vee::pipeline_vert_stage(*vert, "main"),
            vee::pipeline_frag_stage(*frag, "main"),
        },
        .bindings{
            m_quad.vertex_input_bind(),
            vee::vertex_input_bind_per_instance(sizeof(rect)),
            vee::vertex_input_bind_per_instance(sizeof(colour)),
            vee::vertex_input_bind_per_instance(sizeof(uv)),
            vee::vertex_input_bind_per_instance(sizeof(colour)),
        },
        .attributes{
            m_quad.vertex_attribute(0),
            vee::vertex_attribute_vec4(1, 0),
            vee::vertex_attribute_vec4(2, 0),
            vee::vertex_attribute_vec4(3, 0),
            vee::vertex_attribute_vec4(4, 0),
        },
    });
  }
};
} // namespace quack
