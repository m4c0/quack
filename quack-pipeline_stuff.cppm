export module quack:pipeline_stuff;
import :ibatch;
import :objects;
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

  vee::command_pool::type m_cp;

  vee::descriptor_pool desc_pool;
  voo::one_quad m_quad;
  vee::gr_pipeline m_gp;

  static auto create_dset_pool(unsigned max_desc_sets) {
    return vee::create_descriptor_pool(
        max_desc_sets, {vee::combined_image_sampler(max_desc_sets)});
  }

public:
  pipeline_stuff(vee::physical_device pd, vee::command_pool::type cp,
                 vee::render_pass::type rp, unsigned max_batches)
      : m_pd{pd}, m_cp{cp}, desc_pool{create_dset_pool(max_batches)},
        m_quad{pd} {
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

  [[nodiscard]] auto create_batch(unsigned max_insts) {
    auto ds = vee::allocate_descriptor_set(*desc_pool, *dsl);
    return quack::instance_batch{m_pd, m_cp, *pl, ds, 2};
  }

  void run(vee::command_buffer cb, const instance_batch &ib) const {
    int n = ib.build_commands(cb);
    if (n > 0) {
      vee::cmd_bind_gr_pipeline(cb, *m_gp);
      m_quad.run(cb, 0, n);
    }
  }
};
} // namespace quack
