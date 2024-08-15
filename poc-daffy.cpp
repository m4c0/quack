#pragma leco app
#pragma leco add_resource "nasa-jupiter.png"

import casein;
import quack;
import sith;
import sitime;
import vee;
import voo;

extern "C" float sinf(float);

static void update_data(quack::instance *& i, unsigned ms) {
  static int acc {};
  acc += ms;
  float a = sinf(acc / 1000.0f) * 30.0f;
  *i++ = (quack::instance) {
    .position { 0, 0 },
    .size { 1, 1 },
    .uv0 { 0, 0 },
    .uv1 { 1, 1 },
    .multiplier { 1, 1, 1, 1 },
    .rotation { a, 0.5, 0.5 },
  };
}

struct init {
  init() {
    using namespace quack::daffy;

    app_name("poc-daffy");
    clear_colour({ 0.1f, 0.2f, 0.3f, 1.0f });

    add_image("nasa-jupiter.png");

    add_batch(1, 0, update_data);
  }
} i;
