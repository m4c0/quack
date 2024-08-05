module quack;
import dotz;
import hai;
import jute;
import vaselin;

extern "C" 
[[clang::import_module("quack"), clang::import_name("clear_colour")]]
void clear_colour(int r, int g, int b, int a);

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
  auto n = d(g_quads.begin());
}
} // namespace quack::donald
