export module quack:donald;
import :ibatch;
import :pipeline_stuff;
import :upc;
import hai;
import vee;
import voo;

/// Single-batch single-atlas render thread
export namespace quack::donald {
using atlas_t = voo::updater<voo::h2l_image>;
using atlas_fn = atlas_t *(*)(voo::device_and_queue *);
using data_fn = unsigned (*)(mapped_buffers);

void app_name(const char *);
void max_quads(unsigned);

void push_constants(quack::upc);
void atlas(atlas_fn);
void data(data_fn);
} // namespace quack::donald
