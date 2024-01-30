export module quack:pipeline_stuff;
import :ibatch;
import :objects;
import :upc;
import missingno;
import traits;
import vee;
import voo;

namespace quack {
export class pipeline_stuff {
  vee::physical_device m_pd;

  vee::descriptor_set_layout dsl =
      vee::create_descriptor_set_layout({vee::dsl_fragment_sampler()});

  vee::pipeline_layout pl = vee::create_pipeline_layout(
      {*dsl}, {vee::vert_frag_push_constant_range<upc>()});

  vee::descriptor_pool desc_pool;
  voo::one_quad m_quad;
  vee::gr_pipeline m_gp;

  static auto create_dset_pool(unsigned max_desc_sets) {
    return vee::create_descriptor_pool(
        max_desc_sets, {vee::combined_image_sampler(max_desc_sets)});
  }

public:
  pipeline_stuff(const voo::device_and_queue &dq,
                 const voo::swapchain_and_stuff &sw, unsigned max_batches)
      : pipeline_stuff(dq.physical_device(), sw.render_pass(), max_batches) {}
  pipeline_stuff(vee::physical_device pd, vee::render_pass::type rp,
                 unsigned max_batches)
      : m_pd{pd}, desc_pool{create_dset_pool(max_batches)}, m_quad{pd} {
    m_gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = rp,
        .depth_test = false,
        .shaders{
            voo::shader("quack.vert.spv").pipeline_vert_stage(),
            voo::shader("quack.frag.spv").pipeline_frag_stage(),
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

  [[nodiscard]] auto allocate_descriptor_set() {
    return vee::allocate_descriptor_set(*desc_pool, *dsl);
  }
  [[nodiscard]] auto allocate_descriptor_set(vee::image_view::type iv,
                                             vee::sampler::type smp) {
    auto dset = allocate_descriptor_set();
    vee::update_descriptor_set(dset, 0, iv, smp);
    return dset;
  }

  void cmd_bind_descriptor_set(vee::command_buffer cb, vee::descriptor_set ds) {
    vee::cmd_bind_descriptor_set(cb, *pl, 0, ds);
  }

  [[nodiscard]] auto create_batch(unsigned max_insts) {
    return quack::instance_batch{m_pd, *pl, max_insts};
  }

  void run(vee::command_buffer cb, unsigned count) const {
    vee::cmd_bind_gr_pipeline(cb, *m_gp);
    m_quad.run(cb, 0, count);
  }
};
} // namespace quack
