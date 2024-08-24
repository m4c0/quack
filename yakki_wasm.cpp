module quack;
import jute;
import vaselin;

#define IMPORT(R, N) [[clang::import_module("quack_yakki"), clang::import_name(#N)]] extern R N

using namespace quack;
using namespace quack::yakki;

namespace quack::yakki {
  IMPORT(void, start)();

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


static void start_all(void *) {
  if (!on_start || !on_frame) return;

  start();

  res r {};
  on_start(&r);
}

init::init() { vaselin::set_timeout(start_all, nullptr, 0); }
