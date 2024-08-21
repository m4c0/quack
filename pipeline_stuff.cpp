module quack;
import :pipeline_stuff;

void quack::run(quack::pipeline_stuff * ps, const quack::params & p) {
  auto upc = quack::adjust_aspect(*p.pc, p.sw->aspect());
  vee::cmd_set_viewport(p.scb, p.sw->extent());
  vee::cmd_set_scissor(p.scb, p.sw->extent());
  vee::cmd_bind_vertex_buffers(p.scb, 1, p.inst_buffer);
  ps->cmd_bind_descriptor_set(p.scb, p.atlas_dset);
  ps->cmd_push_vert_frag_constants(p.scb, upc);
  ps->run(p.scb, p.count, p.first);
}
