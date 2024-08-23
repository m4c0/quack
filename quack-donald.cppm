export module quack:donald;
import :objects;
import dotz;
import jute;

/// Single-batch single-atlas render thread
export namespace quack::donald {
  void app_name(const char *);
  void max_quads(unsigned);

  dotz::vec2 mouse_pos();

  void clear_colour(dotz::vec4);
  void push_constants(quack::upc);
  void atlas(jute::view res_name);
  void atlas(const void *, unsigned w, unsigned h);
  void data(buffer_fn_t);
} // namespace quack::donald
