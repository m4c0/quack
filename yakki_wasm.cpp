module quack;
import casein;
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
  IMPORT(void, set_grid)(float, float, float, float);
  IMPORT(void, run_batch)(unsigned, unsigned, unsigned, unsigned);

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

    unsigned count() const override { return m_count; }

    dotz::vec2 mouse_pos() const override { return {}; }

    void update_once() {
      auto p = m_buffer.begin();
      m_fn(p);
      m_count = p - m_buffer.begin();
      update_buf(m_idx, m_buffer.begin(), m_count);
    }

  public:
    constexpr buf() = default;
    explicit buf(unsigned i, unsigned sz, buffer_fn_t && fn) : m_idx { i }, m_fn { fn }, m_buffer { sz } {}

    constexpr const auto idx() const { return m_idx; }

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
      g_bufs.back().run_once();
      return &g_bufs.back();
    }
  };

  class rnd : public renderer {
    void run(buffer * yb, image * yi, unsigned count, unsigned first = 0) override {
      auto aspect = casein::window_size.x / casein::window_size.y;
      auto [ gp, gs ] = quack::adjust_aspect(yb->pc(), aspect);
      set_grid(gp.x, gp.y, gs.x, gs.y);

      auto b = static_cast<buf *>(yb);
      auto i = static_cast<img *>(yi);
      run_batch(b->idx(), i->idx, count, first);
    }
    void run(buffer * yb, image * i) override { run(yb, i, yb->count()); }
  };
}

static void render_loop(void *) {
  auto [ r, g, b, a ] = clear_colour;
  clear_canvas(r, g, b, a);

  rnd rr {};
  on_frame(&rr);

  vaselin::request_animation_frame(render_loop, nullptr);
}

static void start_all(void *) {
  if (!on_start || !on_frame) return;

  start();

  res r {};
  on_start(&r);

  vaselin::request_animation_frame(render_loop, nullptr);
}

init::init() { vaselin::request_animation_frame(start_all, nullptr); }
