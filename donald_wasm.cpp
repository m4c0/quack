module quack;
import casein;
import dotz;
import hai;
import jute;
import vaselin;

#define IMPORT(R, N) extern "C" [[clang::import_module("quack"), clang::import_name(#N)]] R N

IMPORT(void, clear_colour)(float, float, float, float);
IMPORT(void, start)();

static const char * g_app_name = "app";
static hai::array<quack::instance> g_quads {};
static quack::upc g_upc {};
static bool g_started {};

namespace quack::donald {
  void app_name(const char * n) { g_app_name = n; }
  void max_quads(unsigned q) { g_quads.set_capacity(q); }

  void clear_colour(dotz::vec4 c) {
    c = dotz::pow(c, 1.0 / 2.2);
    ::clear_colour(c.x, c.y, c.z, c.w);
  }
  void push_constants(quack::upc u) { g_upc = u; }

  dotz::vec2 mouse_pos() {
    auto aspect = casein::window_size.x / casein::window_size.y;
    auto upc = quack::adjust_aspect(g_upc, aspect);
    auto wnd = casein::mouse_pos / casein::window_size;
    auto rel = wnd * 2.0 - 1.0f;
    return rel * upc.grid_size + upc.grid_pos;
  }

  void atlas(jute::view res_name) {}
  void atlas(const void * ptr, unsigned width, unsigned height) {}

  void data(data_fn d) {
    auto i = g_quads.begin();
    auto qty = d(i);

    if (!g_started) {
      start();
      g_started = true;
    }
  }
} // namespace quack::donald
