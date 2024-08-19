#ifndef LECO_TARGET_WASM
#pragma leco app
#pragma leco add_resource "nasa-jupiter.png"
#endif

import quack;
import sitime;

extern "C" float cosf(float);
extern "C" float sinf(float);

static void back(quack::instance *& i) {
  *i++ = {
    .position = { -1 },
    .size = { 2 },
    .colour = { 0, 0, 0, 1 },
  };
}
static void spiral(quack::instance *& i) {
  static sitime::stopwatch time {};

  auto t = time.millis() / 1000.f;
  for (auto n = 0; n < 100; n++) {
    auto a = n * 6.0f * 3.14159265f / 100.0f;
    auto x = a * 0.5f * cosf(a - t);
    auto y = a * 0.5f * sinf(a - t);

    *i++ = (quack::instance) {
      .position { x, y },
      .size { 0.5f, 0.5f },
      .uv0 { 0, 0 },
      .uv1 { 1, 1 },
      .multiplier { 1, 1, 1, 1 },
    };
  }
}

static quack::yakki::buffer * g_bg;
static quack::yakki::buffer * g_u;
static quack::image_updater * g_a;
static void on_start(quack::yakki::resources * r) {
  g_bg = r->buffer(100, back);
  g_bg->pc() = {
    .grid_pos = { 0 },
    .grid_size = { 1 },
  };

  g_u = r->buffer(100, spiral);
  g_u->pc() = {
    .grid_pos = { 0 },
    .grid_size = { 12 },
  };
  g_u->start();

  g_a = r->image("nasa-jupiter.png");
}
static void on_frame(quack::yakki::renderer * r) {
  r->run(g_bg, g_a, 1);
  r->run(g_u, g_a, 100);
}

struct init {
  init() {
    quack::yakki::on_start = on_start;
    quack::yakki::on_frame = on_frame;
  }
} i;
