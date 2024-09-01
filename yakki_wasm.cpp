module quack;
import :wasm;
import casein;
import gelo;
import hai;
import jute;
import vaselin;

#define IMPORT(R, N) [[clang::import_module("quack_yakki"), clang::import_name(#N)]] extern R N

using namespace quack;
using namespace quack::yakki;

namespace quack::yakki {
  IMPORT(unsigned, alloc_text)(const char * name, unsigned sz);

  void (*on_start)(resources *) {};
  void (*on_frame)(renderer *) {};
  dotz::vec4 clear_colour { 0, 0, 0, 1 };
} // namespace quack::yakki

namespace {
  static int u_pos;
  static int u_size;

  class buf : public buffer {
    int m_idx;
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
      gelo::bind_buffer(gelo::ARRAY_BUFFER, m_idx);
      gelo::buffer_data(gelo::ARRAY_BUFFER, m_buffer.begin(), m_count * sizeof(instance), gelo::STATIC_DRAW);
    }

  public:
    constexpr buf() = default;
    explicit buf(int i, unsigned sz, buffer_fn_t && fn) : m_idx { i }, m_fn { fn }, m_buffer { sz } {}

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
    int idx;
  };

  class res : public resources {
    [[nodiscard]] yakki::image * image(jute::view name) override {
      auto t = wasm::create_texture();
      wasm::load_texture(t, name);

      // yes, we are leaking. no, we don't care - they should be finite
      return new img { {}, t };
    }
    [[nodiscard]] yakki::buffer * buffer(unsigned size, buffer_fn_t && fn) override {
      // yes, we are leaking. no, we don't care - they should be finite
      return new buf { wasm::create_buffer(), size, traits::move(fn) };
    }
  };

  class rnd : public renderer {
    void run(buffer * yb, image * yi, unsigned count, unsigned first = 0) override {
      auto aspect = casein::window_size.x / casein::window_size.y;
      auto [ gp, gs ] = quack::adjust_aspect(yb->pc(), aspect);
      gelo::uniform2f(u_pos, gp.x, gp.y);
      gelo::uniform2f(u_size, gs.x, gs.y);

      auto b = static_cast<buf *>(yb);
      auto i = static_cast<img *>(yi);

      gelo::bind_buffer(gelo::ARRAY_BUFFER, b->idx());
      gelo::bind_texture(gelo::TEXTURE_2D, i->idx);
      // TODO: how to deal with "first"?
      gelo::draw_arrays_instanced(gelo::TRIANGLES, 0, 6, count);
    }
    void run(buffer * yb, image * i) override { run(yb, i, yb->count()); }
  };
}

static void render_loop(void *) {
  auto [ r, g, b, a ] = clear_colour;
  gelo::clear_color(r, g, b, a);
  wasm::clear();

  rnd rr {};
  on_frame(&rr);

  vaselin::request_animation_frame(render_loop, nullptr);
}

static void start_all(void *) {
  if (!on_start || !on_frame) return;

  auto [ prog, b ] = wasm::setup();
  u_pos = gelo::get_uniform_location(prog, "pc.grid_pos");
  u_size = gelo::get_uniform_location(prog, "pc.grid_size");

  res r {};
  on_start(&r);

  vaselin::request_animation_frame(render_loop, nullptr);
}

void quack::yakki::start() { vaselin::request_animation_frame(start_all, nullptr); }
