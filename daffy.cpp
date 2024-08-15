module quack;
import hai;
import jute;
import sith;
import sitime;
import voo;

struct batch_pair {
  unsigned max {};
  bool animated {};
  quack::buffer_fn_t fn {};
};
static hai::varray<batch_pair> g_batches { 100 };
static hai::varray<hai::cstr> g_textures { 10 };

static hai::cstr g_app_name = jute::view { "app" }.cstr();

void quack::daffy::app_name(jute::view n) { g_app_name = n.cstr(); }

void quack::daffy::add_batch(unsigned max, void (*fn)(quack::instance *&)) {
  g_batches.push_back(batch_pair { max, false, fn });
}
void quack::daffy::add_batch(unsigned max, void (*fn)(quack::instance *&, unsigned)) {
  using tm = sitime::stopwatch;
  auto wrap = [=, t = tm()](auto *& i) mutable {
    auto dt = t.millis();
    t = {};
    fn(i, dt);
  };

  g_batches.push_back(batch_pair { max, true, wrap });
}

void quack::daffy::add_image(jute::view name) { g_textures.push_back(name.cstr()); }

namespace {
  class renderer : public voo::casein_thread {
  public:
    void run() override {
      voo::device_and_queue dq { g_app_name.begin() };

      quack::pipeline_stuff ps { dq, 100 };

      hai::array<quack::image_updater> ius { g_textures.size() };
      for (auto i = 0; i < g_textures.size(); i++) {
        ius[i] = quack::image_updater { &dq, &ps, [=](auto pd) {
          return voo::load_sires_image(g_textures[i], pd);
        }};
      }

      hai::array<quack::buffer_updater> bus { g_batches.size() };
      hai::array<sith::run_guard> rgs { g_batches.size() };
      for (auto i = 0; i < g_batches.size(); i++) {
        auto b = g_batches[i];
        bus[i] = quack::buffer_updater { &dq, b.max, b.fn };
        if (b.animated) rgs[i] = sith::run_guard { &bus[i] };
      }

      while (!interrupted()) {
        voo::swapchain_and_stuff sw { dq };

        quack::upc rpc {
          .grid_pos = { 0.5f, 0.5f },
          .grid_size = { 4.0f, 4.0f },
        };

        extent_loop(dq.queue(), sw, [&] {
          sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
            auto scb = sw.cmd_render_pass(vee::render_pass_begin {
                .command_buffer = *pcb,
                .clear_color = { { 0, 0, 0, 1 } },
            });
            for (auto & u : bus) {
              ps.run({
                  .sw = &sw,
                  .scb = *scb,
                  .pc = &rpc,
                  .inst_buffer = u.data().local_buffer(),
                  .atlas_dset = ius[0].dset(),
                  .count = u.count(),
              });
            }
          });
        });
      }
    }
  } r;
} // namespace
