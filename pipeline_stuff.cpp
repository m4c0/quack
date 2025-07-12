#pragma leco add_shader "quack.frag"
#pragma leco add_shader "quack.vert"

module quack;
import :pipeline_stuff;

quack::pipeline_stuff::pipeline_stuff(const voo::device_and_queue & dq, unsigned max_dsets) :
  desc_pool { create_dset_pool(max_dsets) },
  m_quad { dq.physical_device() },
  m_rp { voo::single_att_render_pass(dq) }
{
  m_gp = vee::create_graphics_pipeline({
      .pipeline_layout = *pl,
      .render_pass = *m_rp,
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
    auto dwh = sev2.x - sev2.y;
    auto asp_d = dotz::max({ 0 }, dotz::vec2 { dwh, -dwh });
    auto scr_sq = sev2 - asp_d;

    auto [gp, gs] = p.scissor.ref ? *p.scissor.ref : *p.pc;

    auto e = scr_sq * pe / gs;
    sc.extent = { static_cast<unsigned>(e.x), static_cast<unsigned>(e.y) };

    auto po = p.scissor.offset;
    auto tl = gp - gs / 2.0f;
    auto o = scr_sq * (po - tl) / gs + asp_d / 2.0f;
    sc.offset = { static_cast<int>(o.x), static_cast<int>(o.y) };
  }

  auto cb = p.sw->command_buffer();
  auto upc = quack::adjust_aspect(*p.pc, p.sw->aspect());
  vee::cmd_set_viewport(cb, p.sw->extent());
  vee::cmd_set_scissor(cb, sc);
  vee::cmd_bind_vertex_buffers(cb, 1, p.inst_buffer);
  ps->cmd_bind_descriptor_set(cb, p.atlas_dset);
  ps->cmd_push_vert_frag_constants(cb, upc);
  ps->run(cb, p.count, p.first);
}
