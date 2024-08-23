#pragma leco add_shader "quack.frag"
#pragma leco add_shader "quack.vert"

module quack;
import :pipeline_stuff;

quack::pipeline_stuff::pipeline_stuff(vee::physical_device pd, vee::render_pass::type rp, unsigned max_dsets)
    : desc_pool { create_dset_pool(max_dsets) }
    , m_quad { pd } {
  m_gp = vee::create_graphics_pipeline({
      .pipeline_layout = *pl,
      .render_pass = rp,
      .depth_test = false,
      .shaders {
          voo::shader("quack.vert.spv").pipeline_vert_stage(),
          voo::shader("quack.frag.spv").pipeline_frag_stage(),
      },
      .bindings {
          m_quad.vertex_input_bind(),
          vee::vertex_input_bind_per_instance(sizeof(instance)),
      },
      .attributes {
          m_quad.vertex_attribute(0),
          vee::vertex_attribute_vec4(1, 0),
          vee::vertex_attribute_vec4(1, 8 * sizeof(float)),
          vee::vertex_attribute_vec4(1, 4 * sizeof(float)),
          vee::vertex_attribute_vec4(1, 12 * sizeof(float)),
          vee::vertex_attribute_vec4(1, 16 * sizeof(float)),
      },
  });
}

void quack::run(quack::pipeline_stuff * ps, const quack::params & p) {
  auto se = p.sw->extent();
  vee::rect sc { .offset = {}, .extent = se };

  auto pe = p.scissor.extent;
  if (dotz::length(pe) > 0) {
    dotz::vec2 sev2 { se.width, se.height };
    auto dwh = (sev2.x - sev2.y) / 2.0f;
    auto asp_d = dotz::max({ 0 }, dotz::vec2 { dwh, -dwh });

    auto [gp, gs] = p.scissor.ref ? *p.scissor.ref : *p.pc;

    auto e = sev2 * pe / gs - asp_d;
    sc.extent = { static_cast<unsigned>(e.x), static_cast<unsigned>(e.y) };

    auto po = p.scissor.offset;
    auto tl = gp - gs / 2.0f;
    auto o = sev2 * (po - tl) / gs + asp_d / 2.0f;
    sc.offset = { static_cast<int>(o.x), static_cast<int>(o.y) };
  }

  auto upc = quack::adjust_aspect(*p.pc, p.sw->aspect());
  vee::cmd_set_viewport(p.scb, p.sw->extent());
  vee::cmd_set_scissor(p.scb, sc);
  vee::cmd_bind_vertex_buffers(p.scb, 1, p.inst_buffer);
  ps->cmd_bind_descriptor_set(p.scb, p.atlas_dset);
  ps->cmd_push_vert_frag_constants(p.scb, upc);
  ps->run(p.scb, p.count, p.first);
}
