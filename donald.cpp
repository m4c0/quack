module quack;
import :ibatch;
import :pipeline_stuff;
import :upc;
import dotz;
import hai;
import sith;
import vee;
import voo;

namespace {
class thread : public voo::casein_thread {
public:
  void run() override;
};

class atlas_updater : public voo::updater<voo::h2l_image> {
  vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
  vee::physical_device m_pd;
  vee::descriptor_set m_dset;

  void update_data(voo::h2l_image *img) override;

public:
  atlas_updater(voo::device_and_queue *dq, vee::descriptor_set dset)
      : updater{dq->queue(), {}}
      , m_pd{dq->physical_device()}
      , m_dset{dset} {}
};
} // namespace

static const char *g_app_name = "app";
static unsigned g_max_quads = 0;
static quack::upc g_upc{};
static quack::donald::atlas_fn g_atlas_fn{};
static quack::donald::data_fn g_data_fn{};
static dotz::vec4 g_clear_colour{0.1f, 0.2f, 0.3f, 1.0f};

// TODO: sync count change with data change
static unsigned g_quads = 0;
static atlas_updater *g_atlas;
static quack::instance_batch_thread *g_batch;

static void update(quack::instance *all) { g_quads = g_data_fn(all); }

void atlas_updater::update_data(voo::h2l_image *img) {
  *img = g_atlas_fn(m_pd);
  vee::update_descriptor_set(m_dset, 0, img->iv(), *m_smp);
}

void thread::run() {
  voo::device_and_queue dq{g_app_name};
  quack::pipeline_stuff ps{dq, 1};

  auto dset = ps.allocate_descriptor_set();

  quack::instance_batch_thread ib{dq.queue(), ps.create_batch(g_max_quads),
                                  update};
  ib.run_once();
  g_batch = &ib;

  atlas_updater atlas{&dq, dset};
  atlas.run_once();
  g_atlas = &atlas;

  release_init_lock();

  while (!interrupted()) {
    voo::swapchain_and_stuff sw{dq};

    extent_loop(dq.queue(), sw, [&] {
      auto upc = quack::adjust_aspect(g_upc, sw.aspect());
      sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
        auto scb = sw.cmd_render_pass(vee::render_pass_begin{
            .command_buffer = *pcb,
            .clear_color = {{g_clear_colour.x, g_clear_colour.y,
                             g_clear_colour.z, g_clear_colour.w}},
        });
        vee::cmd_set_viewport(*scb, sw.extent());
        vee::cmd_set_scissor(*scb, sw.extent());
        ib.data().build_commands(*pcb);
        ps.cmd_bind_descriptor_set(*scb, dset);
        ps.cmd_push_vert_frag_constants(*scb, upc);
        ps.run(*scb, g_quads);
      });
    });
  }
}

static thread &t() {
  static thread i{};
  return i;
}

namespace quack::donald {
void app_name(const char *n) { g_app_name = n; }
void max_quads(unsigned q) { g_max_quads = q; }

void clear_colour(dotz::vec4 c) { g_clear_colour = c; }
void push_constants(quack::upc u) { g_upc = u; }
void atlas(atlas_fn a) {
  // TODO: the function is wrong. It creates the updater, but it should instead
  // create the image
  g_atlas_fn = a;
  if (g_atlas) {
    g_atlas->run_once();
  } else if (!g_batch) {
    t();
  }
}
void data(data_fn d) {
  g_data_fn = d;
  if (g_batch) {
    g_batch->run_once();
  } else if (!g_atlas) {
    t();
  }
}
} // namespace quack::donald
