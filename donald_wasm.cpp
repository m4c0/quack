module quack;
import dotz;
import jute;
import vaselin;

static const char *g_app_name = "app";
static dotz::vec4 g_clear_colour{0.1f, 0.2f, 0.3f, 1.0f};
static unsigned g_max_quads = 0;
static quack::upc g_upc{};

namespace quack::donald {
void app_name(const char *n) { g_app_name = n; }
void max_quads(unsigned q) { g_max_quads = q; }

void clear_colour(dotz::vec4 c) { g_clear_colour = c; }
void push_constants(quack::upc u) { g_upc = u; }

void atlas(jute::view res_name) {}
void atlas(const void *ptr, unsigned width, unsigned height) {}

void data(data_fn d) {}
} // namespace quack::donald
