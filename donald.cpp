module quack;
import :ibatch;
import :pipeline_stuff;
import :upc;
import hai;
import sith;
import vee;
import voo;

void quack::donald::run() {
  voo::device_and_queue dq{app_name()};
  pipeline_stuff ps{dq, 1};

  auto atlas = create_atlas(&dq);
  atlas->run_once();
  m_atlas = &*atlas;

  donald_ibt ib{dq.queue(), ps, max_quads(), this};
  ib.run_once();
  m_batch = &ib;

  auto smp = create_sampler();
  auto dset = ps.allocate_descriptor_set(atlas->data().iv(), *smp);

  release_init_lock();

  while (!interrupted()) {
    voo::swapchain_and_stuff sw{dq};

    extent_loop(dq.queue(), sw, [&] {
      auto upc = quack::adjust_aspect(push_constants(), sw.aspect());
      sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
        auto scb = sw.cmd_render_pass(pcb);
        vee::cmd_set_viewport(*scb, sw.extent());
        vee::cmd_set_scissor(*scb, sw.extent());
        ib.data().build_commands(*pcb);
        ps.cmd_bind_descriptor_set(*scb, dset);
        ps.cmd_push_vert_frag_constants(*scb, upc);
        ps.run(*scb, quad_count());
      });
    });
  }
}
