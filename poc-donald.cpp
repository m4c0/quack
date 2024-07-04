#pragma leco app
#pragma leco add_resource "nasa-jupiter.png"
import casein;
import quack;
import sitime;
import voo;

static quack::donald::atlas_t *jupiter(voo::device_and_queue *dq) {
  return new voo::sires_image{"nasa-jupiter.png", dq};
}

static unsigned quads_4x4(quack::mapped_buffers all) {
  static sitime::stopwatch t{};

  auto angle = 360.f * t.millis() / 1000.f;

  auto [c, m, p, u, r] = all;
  for (auto y = 0, i = 0; y < 4; y++) {
    for (auto x = 0; x < 4; x++, i++) {
      float xf = x + 0.05;
      float yf = y + 0.05;
      c[i] = {1, 1, 1, 1};
      m[i] = {1, 1, 1, 1};
      p[i] = {{xf, yf}, {0.9, 0.9}};
      u[i] = {{0, 0}, {1, 1}};
      r[i] = {angle, 0.5, 0.5};
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

    clear_colour({0.1, 0.15, 0.1, 1});
    push_constants({
        .grid_pos = {2, 2},
        .grid_size = {4, 4},
    });
    atlas(jupiter);
    data(quads_4x4);

    casein::handle(casein::REPAINT, repaint);
  }
} i;
