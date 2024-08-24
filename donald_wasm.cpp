module quack;
import casein;
import dotz;
import hai;
import jute;
import sires;
import vaselin;

#define IMPORT(R, N) extern "C" [[clang::import_module("quack"), clang::import_name(#N)]] R N

IMPORT(void, clear_colour)(float, float, float, float);
IMPORT(void, bind_instances)(unsigned, const void *, unsigned);
IMPORT(void, load_texture)(const char *, unsigned);
IMPORT(void, set_grid)(float, float, float, float);
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
  void push_constants(quack::upc u) {
    auto aspect = casein::window_size.x / casein::window_size.y;
    u = quack::adjust_aspect(u, aspect);

    set_grid(u.grid_pos.x, u.grid_pos.y, u.grid_size.x, u.grid_size.y);
    g_upc = u;
  }

  dotz::vec2 mouse_pos() {
    auto wnd = casein::mouse_pos / casein::window_size;
    auto rel = wnd * 2.0 - 1.0f;
    return rel * g_upc.grid_size + g_upc.grid_pos;
  }

  void atlas(jute::view res_name) { load_texture(res_name.begin(), res_name.size()); }
  void atlas(const void * ptr, unsigned width, unsigned height) {}

  void data(buffer_fn_t d) {
    auto i = g_quads.begin();
    d(i);
    auto qty = i - g_quads.begin();

    if (qty > 0) bind_instances(qty, g_quads.begin(), g_quads.size());

    if (!g_started) {
      start();
      g_started = true;
    }
  }
} // namespace quack::donald
