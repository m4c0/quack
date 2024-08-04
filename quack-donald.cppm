export module quack:donald;
import :objects;
import :upc;
import dotz;
import vee;
import voo;

/// Single-batch single-atlas render thread
export namespace quack::donald {
using atlas_t = voo::h2l_image;
using atlas_fn = atlas_t (*)(vee::physical_device);
using data_fn = unsigned (*)(instance *);

void app_name(const char *);
void max_quads(unsigned);

void clear_colour(dotz::vec4);
void push_constants(quack::upc);
void atlas(atlas_fn);
void data(data_fn);
} // namespace quack::donald
