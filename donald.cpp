module quack;
import :ibatch;
import :pipeline_stuff;
import :upc;
import dotz;
import hai;
import jute;
import silog;
import sith;
import traits;
import vee;
import voo;

namespace {
using atlas_t = voo::h2l_image;
using atlas_fn = atlas_t (*)(vee::physical_device);

class thread : public voo::casein_thread {
public:
  void run() override;
};

class atlas_updater : public voo::updater<voo::h2l_image> {
  vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
  vee::physical_device m_pd;
  vee::descriptor_set m_dset;

  voo::h2l_image m_old{};
  vee::descriptor_set m_dset_old;

  void update_data(voo::h2l_image *img) override;

public:
  atlas_updater(voo::device_and_queue *dq, quack::pipeline_stuff *ps)
      : updater{dq->queue(), {}}
      , m_pd{dq->physical_device()}
      , m_dset{ps->allocate_descriptor_set()}
      , m_dset_old{ps->allocate_descriptor_set()} {}

  [[nodiscard]] constexpr auto dset() const { return m_dset; }
};
} // namespace

static const char *g_app_name = "app";
static unsigned g_max_quads = 0;
static quack::upc g_upc{};
static atlas_fn g_atlas_fn{};
static quack::donald::data_fn g_data_fn{};
static dotz::vec4 g_clear_colour{0.1f, 0.2f, 0.3f, 1.0f};

// TODO: sync count change with data change
static unsigned g_quads = 0;
static atlas_updater *g_atlas;
static quack::instance_batch_thread *g_batch;

static void update(quack::instance *all) {
  if (g_data_fn) {
    g_quads = g_data_fn(all);
  } else {
    g_quads = 0;
    silog::log(silog::warning, "No sprite data defined");
  }
}

void atlas_updater::update_data(voo::h2l_image *img) {
  m_old = traits::move(*img);

  if (g_atlas_fn) {
    *img = g_atlas_fn(m_pd);
  } else {
    *img = voo::h2l_image{m_pd, 16, 16};
    silog::log(silog::warning, "No atlas defined");
  }

  auto tmp = m_dset_old;
  m_dset_old = m_dset;
  m_dset = tmp;

  vee::update_descriptor_set(m_dset, 0, img->iv(), *m_smp);
}

void thread::run() {
  voo::device_and_queue dq{g_app_name};
  quack::pipeline_stuff ps{dq, 2};

  quack::instance_batch_thread ib{dq.queue(), ps.create_batch(g_max_quads),
                                  update};
  ib.run_once();
  g_batch = &ib;

  atlas_updater atlas{&dq, &ps};
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
        ps.cmd_bind_descriptor_set(*scb, atlas.dset());
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
void atlas(jute::view res_name) {
  static jute::view name;
  name = res_name;
  atlas([](auto pd) { return voo::load_sires_image(name, pd); });
}
void atlas(const void *ptr, unsigned width, unsigned height) {
  static const void *p;
  static unsigned w;
  static unsigned h;
  p = ptr;
  w = width;
  h = height;
  atlas([](auto pd) {
    voo::h2l_image img{pd, w, h};

    voo::mapmem m{img.host_memory()};
    auto *c = static_cast<unsigned *>(*m);
    auto *o = static_cast<const unsigned *>(p);
    for (auto i = 0; i < w * h; i++)
      *c++ = *o++;

    return img;
  });
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
