#pragma leco app
#pragma leco add_resource "nasa-jupiter.png"
import casein;
import quack;
import sitime;

static unsigned quads_4x4(quack::instance *i) {
  static sitime::stopwatch t{};

  auto angle = 360.f * t.millis() / 1000.f;

  for (auto y = 0; y < 4; y++) {
    for (auto x = 0; x < 4; x++, i++) {
      float xf = x + 0.05;
      float yf = y + 0.05;
      i->colour = {1, 1, 1, 1};
      i->multiplier = {1, 1, 1, 1};
      i->position = {xf, yf};
      i->size = {0.9f, 0.9f};
      i->uv0 = {0, 0};
      i->uv1 = {1, 1};
      i->rotation = {angle, 0.5, 0.5};
    }
  }
  return 16;
}

static void repaint() { quack::donald::data(quads_4x4); }

struct init {
  init() {
    using namespace quack::donald;

    app_name("quack-donald-poc");
    max_quads(16);

    clear_colour({0.1f, 0.15f, 0.1f, 1.f});
    push_constants({
        .grid_pos = {2, 2},
        .grid_size = {4, 4},
    });
    atlas("nasa-jupiter.png");
    data(quads_4x4);

    casein::handle(casein::REPAINT, repaint);
  }
} i;
