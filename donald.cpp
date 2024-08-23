module quack;
import :pipeline_stuff;
import casein;
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
} // namespace

static const char * g_app_name = "app";
static unsigned g_max_quads = 0;
static quack::upc g_upc {};
static atlas_fn g_atlas_fn = [](auto pd) {
  silog::log(silog::warning, "No atlas defined");
  return voo::h2l_image { pd, 16, 16 };
};
static quack::buffer_fn_t g_data_fn {};
static dotz::vec4 g_clear_colour { 0.1f, 0.2f, 0.3f, 1.0f };

// TODO: sync count change with data change
static unsigned g_quads = 0;
static quack::image_updater * g_atlas;
static quack::buffer_updater * g_batch;

static void update(quack::instance *& all) {
  if (g_data_fn) {
    auto orig = all;
    g_data_fn(all);
    g_quads = all - orig;
    if (g_quads < 0) {
      g_quads = 0;
      silog::log(silog::warning, "Invalid sprite data defined");
    }
  } else {
    g_quads = 0;
    silog::log(silog::warning, "No sprite data defined");
  }
}

void thread::run() {
  voo::device_and_queue dq { g_app_name };
  quack::pipeline_stuff ps { dq, 2 };

  quack::buffer_updater ib { &dq, g_max_quads, update };
  g_batch = &ib;

  quack::image_updater atlas { &dq, &ps, [](auto pd) { return g_atlas_fn(pd); } };
  g_atlas = &atlas;

  release_init_lock();

  while (!interrupted()) {
    voo::swapchain_and_stuff sw { dq };

    extent_loop(dq.queue(), sw, [&] {
      auto upc = quack::adjust_aspect(g_upc, sw.aspect());
      sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
        auto scb = sw.cmd_render_pass(vee::render_pass_begin {
            .command_buffer = *pcb,
            .clear_color = { { g_clear_colour.x, g_clear_colour.y, g_clear_colour.z, g_clear_colour.w } },
        });
        vee::cmd_set_viewport(*scb, sw.extent());
        vee::cmd_set_scissor(*scb, sw.extent());
        vee::cmd_bind_vertex_buffers(*scb, 1, ib.data().local_buffer());
        ps.cmd_bind_descriptor_set(*scb, atlas.dset());
        ps.cmd_push_vert_frag_constants(*scb, upc);
        ps.run(*scb, g_quads);
      });
    });
  }
}

static thread & t() {
  static thread i {};
  return i;
}

namespace quack::donald {
  void app_name(const char * n) { g_app_name = n; }
  void max_quads(unsigned q) { g_max_quads = q; }

  void clear_colour(dotz::vec4 c) { g_clear_colour = c; }
  void push_constants(quack::upc u) { g_upc = u; }

  dotz::vec2 mouse_pos() {
    auto aspect = casein::window_size.x / casein::window_size.y;
    auto upc = quack::adjust_aspect(g_upc, aspect);
    auto wnd = casein::mouse_pos / casein::window_size;
    auto rel = wnd * 2.0 - 1.0f;
    return rel * upc.grid_size + upc.grid_pos;
  }

  void atlas(atlas_fn a) {
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
  void atlas(const void * ptr, unsigned width, unsigned height) {
    static const void * p;
    static unsigned w;
    static unsigned h;
    p = ptr;
    w = width;
    h = height;
    atlas([](auto pd) {
      voo::h2l_image img { pd, w, h };

      voo::mapmem m { img.host_memory() };
      auto * c = static_cast<unsigned *>(*m);
      auto * o = static_cast<const unsigned *>(p);
      for (auto i = 0; i < w * h; i++) *c++ = *o++;

      return img;
    });
  }
  void data(buffer_fn_t d) {
    g_data_fn = d;
    if (g_batch) {
      g_batch->run_once();
    } else if (!g_atlas) {
      t();
    }
  }
} // namespace quack::donald
