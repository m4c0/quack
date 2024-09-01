module quack;
import :wasm;
import casein;
import dotz;
import gelo;
import hai;
import jute;
import sires;
import vaselin;

#define IMPORT(R, N) [[clang::import_module("quack_donald"), clang::import_name(#N)]] extern R N

static const char * g_app_name = "app";
static hai::array<quack::instance> g_quads {};
static quack::upc g_upc {};
static bool g_started {};

static int g_u_pos;
static int g_u_size;
static int g_buffer;
static int g_texture;
static unsigned g_qty {};

static jute::view g_atlas {};

static void draw(void *) {
  quack::wasm::clear();
  gelo::uniform2f(g_u_pos, g_upc.grid_pos.x, g_upc.grid_pos.y);
  gelo::uniform2f(g_u_size, g_upc.grid_size.x, g_upc.grid_size.y);
  gelo::draw_arrays_instanced(gelo::TRIANGLES, 0, 6, g_qty);
  vaselin::request_animation_frame(draw, nullptr);
}
static void load_atlas(void *) {
  quack::wasm::load_texture(g_texture, g_atlas); 
}

namespace quack::donald {
  void app_name(const char * n) { g_app_name = n; }
  void max_quads(unsigned q) { g_quads.set_capacity(q); }

  void clear_colour(dotz::vec4 c) {
    c = dotz::pow(c, 1.0 / 2.2);
    gelo::clear_color(c.x, c.y, c.z, c.w);
  }
  void push_constants(quack::upc u) {
    auto aspect = casein::window_size.x / casein::window_size.y;
    g_upc = quack::adjust_aspect(u, aspect);
  }

  dotz::vec2 mouse_pos() {
    auto wnd = casein::mouse_pos / casein::window_size;
    auto rel = wnd * 2.0 - 1.0f;
    return rel * g_upc.grid_size + g_upc.grid_pos;
  }

  void atlas(jute::view res_name) { 
    g_atlas = res_name;
    vaselin::request_animation_frame(load_atlas, nullptr);
  }
  void atlas(const void * ptr, unsigned width, unsigned height) {}

  void data(buffer_fn_t d) {
    if (!g_started) {
      auto x = wasm::setup();
      g_u_pos = x.u_grid_pos;
      g_u_size = x.u_grid_size;
      g_buffer = wasm::create_buffer();
      g_texture = wasm::create_texture();
      g_started = true;

      vaselin::request_animation_frame(draw, nullptr);
    }

    auto i = g_quads.begin();
    d(i);
    auto qty = i - g_quads.begin();

    if (qty > 0) {
      gelo::bind_buffer(gelo::ARRAY_BUFFER, g_buffer);
      gelo::buffer_data(gelo::ARRAY_BUFFER, g_quads.begin(), g_quads.size() * sizeof(instance), gelo::STATIC_DRAW);
      g_qty = qty;
    }
  }
} // namespace quack::donald
