module quack;
import dotz;
import hai;
import jute;
import vaselin;

#define IMPORT(R, N)                                                        \
  extern "C" [[clang::import_module("quack"), clang::import_name(#N)]] R N

IMPORT(void, clear_colour)(int r, int g, int b, int a);
IMPORT(void, fill_colour)(int r, int g, int b, int a);
IMPORT(void, fill_rect)(float, float, float, float);
IMPORT(void, restore)();
IMPORT(void, save)();
IMPORT(void, scale)(float, float);
IMPORT(void, translate)(float, float);

static const char *g_app_name = "app";
static hai::array<quack::instance> g_quads{};
static quack::upc g_upc{};

namespace quack::donald {
void app_name(const char *n) { g_app_name = n; }
void max_quads(unsigned q) { g_quads.set_capacity(q); }

void clear_colour(dotz::vec4 c) { 
  c = c * 256;
  ::clear_colour(c.x, c.y, c.z, c.w);
}
void push_constants(quack::upc u) { g_upc = u; }

void atlas(jute::view res_name) {}
void atlas(const void *ptr, unsigned width, unsigned height) {}

void data(data_fn d) {
  auto i = g_quads.begin();
  auto qty = d(i);

  save();

  auto s = dotz::vec2{800, 600} / (g_upc.grid_size * 4.0f);
  scale(s.x, s.y);
  translate(-1 + g_upc.grid_pos.x, -2 + g_upc.grid_pos.y);

  for (auto n = 0; n < qty; n++, i++) {
    fill_colour( i->colour.x, i->colour.y, i->colour.z, i->colour.w);

    fill_rect(i->position.x, i->position.y, i->size.x, i->size.y);
  }

  restore();
}
} // namespace quack::donald
