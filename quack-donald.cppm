export module quack:donald;
import :objects;
import :upc;
import dotz;
import jute;

/// Single-batch single-atlas render thread
export namespace quack::donald {
using data_fn = unsigned (*)(instance *);

void app_name(const char *);
void max_quads(unsigned);

void clear_colour(dotz::vec4);
void push_constants(quack::upc);
void atlas(jute::view res_name);
void atlas(const void *, unsigned w, unsigned h);
void data(data_fn);
} // namespace quack::donald
