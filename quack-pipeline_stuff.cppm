export module quack:pipeline_stuff;
import :objects;
import missingno;
import traits;
import vee;
import voo;

namespace quack {
  export class pipeline_stuff {
    vee::descriptor_set_layout dsl = vee::create_descriptor_set_layout({ vee::dsl_fragment_sampler() });

    vee::pipeline_layout pl = vee::create_pipeline_layout(*dsl, vee::vert_frag_push_constant_range<upc>());

    vee::descriptor_pool desc_pool;
    voo::one_quad m_quad;
    vee::render_pass m_rp;
    vee::gr_pipeline m_gp;

    static auto create_dset_pool(unsigned max_desc_sets) {
      return vee::create_descriptor_pool(max_desc_sets, { vee::combined_image_sampler(max_desc_sets) });
    }

  public:
    pipeline_stuff(const voo::device_and_queue & dq, unsigned max_dsets);

    [[nodiscard]] auto allocate_descriptor_set() { return vee::allocate_descriptor_set(*desc_pool, *dsl); }
    [[nodiscard]] auto allocate_descriptor_set(vee::image_view::type iv, vee::sampler::type smp) {
      auto dset = allocate_descriptor_set();
      vee::update_descriptor_set(dset, 0, iv, smp);
      return dset;
    }

    void cmd_bind_descriptor_set(vee::command_buffer cb, vee::descriptor_set ds) const {
      vee::cmd_bind_descriptor_set(cb, *pl, 0, ds);
    }
    void cmd_push_vert_frag_constants(vee::command_buffer cb, const upc & pc) const {
      vee::cmd_push_vert_frag_constants(cb, *pl, &pc);
    }

    void run(vee::command_buffer cb, unsigned count, unsigned first = 0) const {
      vee::cmd_bind_gr_pipeline(cb, *m_gp);
      m_quad.run(cb, 0, count, first);
    }
  };
} // namespace quack
