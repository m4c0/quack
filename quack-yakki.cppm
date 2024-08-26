export module quack:yakki;
import :objects;
import dotz;
import jute;

export namespace quack::yakki {
  struct buffer {
    [[nodiscard]] virtual upc & pc() = 0;
    [[nodiscard]] virtual quack::scissor & scissor() = 0;
    [[nodiscard]] virtual unsigned count() const = 0;
    [[nodiscard]] virtual dotz::vec2 mouse_pos() const = 0;
    virtual void start() = 0;
    virtual void run_once() = 0;
  };
  struct image {};

  struct resources {
    [[nodiscard]] virtual image * image(jute::view name) = 0;
    [[nodiscard]] virtual buffer * buffer(unsigned size, buffer_fn_t && fn) = 0;
  };

  struct renderer {
    virtual void run(buffer * b, image * i, unsigned count, unsigned first = 0) = 0;
    virtual void run(buffer * b, image * i) = 0;
  };

  extern void (*on_start)(resources *);
  extern void (*on_frame)(renderer *);
  extern dotz::vec4 clear_colour;

  // Exists with the sole purpose of avoid stealing events from other ducks
  void start();
} // namespace quack::yakki

#ifdef LECO_TARGET_WASM
#pragma leco add_impl yakki_wasm
#else
#pragma leco add_impl yakki
#endif
