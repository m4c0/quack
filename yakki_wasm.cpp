module quack;
import hai;
import jute;
import vaselin;

#define IMPORT(R, N) [[clang::import_module("quack_yakki"), clang::import_name(#N)]] extern R N

using namespace quack;
using namespace quack::yakki;

namespace quack::yakki {
  IMPORT(void, start)();
  IMPORT(unsigned, alloc_buf)();
  IMPORT(unsigned, alloc_text)(const char * name, unsigned sz);

  void (*on_start)(resources *) {};
  void (*on_frame)(renderer *) {};
  dotz::vec4 clear_colour { 0, 0, 0, 1 };
} // namespace quack::yakki

namespace {
  struct init { init(); } i;

  class buf : public buffer {
    unsigned m_idx;

    upc m_pc {};
    quack::scissor m_scissor {};

    upc & pc() override { return m_pc; }
    quack::scissor & scissor() override { return m_scissor; }

    void start() override {}
    void run_once() override {}

    unsigned count() const override { return 0; }

    dotz::vec2 mouse_pos() const override { return {}; }
  public:
    constexpr buf() = default;
    explicit buf(unsigned i) : m_idx { i } {}
  };
  struct img : public image {
    unsigned idx;
  };

  hai::varray<img> g_imgs { 16 };
  hai::varray<buf> g_bufs { 128 };

  class res : public resources {
    [[nodiscard]] yakki::image * image(jute::view name) override {
      auto idx = alloc_text(name.begin(), name.size());

      g_imgs.push_back(img { {}, idx });
      return &g_imgs.back();
    }
    [[nodiscard]] yakki::buffer * buffer(unsigned size, buffer_fn_t && fn) override {
      g_bufs.push_back(buf { alloc_buf() });
      return &g_bufs.back();
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
