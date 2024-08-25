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
  IMPORT(void, update_buf)(unsigned, const void *, unsigned);
  IMPORT(void, clear_canvas)(float, float, float, float);

  void (*on_start)(resources *) {};
  void (*on_frame)(renderer *) {};
  dotz::vec4 clear_colour { 0, 0, 0, 1 };
} // namespace quack::yakki

namespace {
  struct init { init(); } i;

  class buf : public buffer {
    unsigned m_idx;
    buffer_fn_t m_fn;
    unsigned m_count {};
    hai::array<instance> m_buffer;

    upc m_pc {};
    quack::scissor m_scissor {};

    upc & pc() override { return m_pc; }
    quack::scissor & scissor() override { return m_scissor; }

    void start() override {
      vaselin::request_animation_frame([](void * p) {
        static_cast<buf *>(p)->update_once();
        static_cast<buf *>(p)->start();
      }, this);
    }
    void run_once() override {
      vaselin::request_animation_frame([](void * p) {
        static_cast<buf *>(p)->update_once();
      }, this);
    }

    unsigned count() const override { return m_count; }

    dotz::vec2 mouse_pos() const override { return {}; }

    void update_once() {
      auto p = m_buffer.begin();
      m_fn(p);
      auto qty = p - m_buffer.begin();
      update_buf(m_idx, m_buffer.begin(), qty);
    }

  public:
    constexpr buf() = default;
    explicit buf(unsigned i, unsigned sz, buffer_fn_t && fn) : m_idx { i }, m_fn { fn }, m_buffer { sz } {}
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
      g_bufs.push_back(buf { alloc_buf(), size, traits::move(fn) });
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

  vaselin::request_animation_frame([](void *) {
    auto [ r, g, b, a ] = clear_colour;
    clear_canvas(r, g, b, a);
  }, nullptr);
}

init::init() { vaselin::request_animation_frame(start_all, nullptr); }
