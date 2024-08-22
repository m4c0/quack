export module quack;
export import :donald;
export import :objects;
export import :upc;

#ifdef LECO_TARGET_WASM
#pragma leco add_impl donald_wasm
#else
export import :pipeline_stuff;
export import :updater;
export import :yakki;
import dotz;
import vee;
import voo;

export namespace quack {
  struct rect {
    dotz::vec2 offset;
    dotz::vec2 extent;
  };
  struct params {
    voo::swapchain_and_stuff * sw;
    vee::command_buffer scb;
    quack::upc * pc;
    vee::buffer::type inst_buffer;
    vee::descriptor_set atlas_dset;
    unsigned count;
    unsigned first {};
    rect scissor {};
  };
  void run(pipeline_stuff * ps, const params & p);
} // namespace quack

#pragma leco add_impl donald pipeline_stuff updater yakki
#endif
