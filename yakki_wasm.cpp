module quack;
import casein;
import jute;

using namespace quack;
using namespace quack::yakki;

namespace quack::yakki {
  void (*on_start)(resources *) {};
  void (*on_frame)(renderer *) {};
  dotz::vec4 clear_colour { 0, 0, 0, 1 };
} // namespace quack::yakki

namespace {
  struct init { init(); } i;

  class buf : public buffer {};
  class img : public image {};

  class res : public resources {
    [[nodiscard]] yakki::image * image(jute::view name) override {
      return nullptr;
    }
    [[nodiscard]] yakki::buffer * buffer(unsigned size, buffer_fn_t && fn) override {
      return nullptr;
    }
  };

  class rnd : public renderer {
    void run(buffer * yb, image * yi, unsigned count, unsigned first = 0) override {
    }
    void run(buffer * yb, image * i) override {
    }
  };
}


static void start() {
  res r {};
  on_start(&r);
}

init::init() { casein::handle(casein::CREATE_WINDOW, start); }
