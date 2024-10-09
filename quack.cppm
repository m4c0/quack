export module quack;
export import :donald;
export import :objects;
export import :yakki;
import casein;
import dotz;

#ifdef LECO_TARGET_WASM
#pragma leco add_impl donald_wasm
#else
export import :pipeline_stuff;
export import :updater;
import vee;
import voo;

export namespace quack {
  struct params {
    voo::swapchain_and_stuff * sw;
    vee::command_buffer scb;
    quack::upc * pc;
    vee::buffer::type inst_buffer;
    vee::descriptor_set atlas_dset;
    unsigned count;
    unsigned first {};
    quack::scissor scissor {};
  };
  void run(pipeline_stuff * ps, const params & p);
} // namespace quack

#pragma leco add_impl donald pipeline_stuff updater
#endif

export namespace quack {
[[nodiscard]] constexpr upc adjust_aspect(upc u, float aspect) {
  auto g = u.grid_size / 2.0;
  float grid_aspect = g.x / g.y;
  u.grid_size = grid_aspect < aspect ? dotz::vec2{aspect * g.y, g.y}
                                     : dotz::vec2{g.x, g.x / aspect};
  return u;
}
[[nodiscard]] dotz::vec2 mouse_pos(const upc & pc) {
  auto aspect = casein::window_size.x / casein::window_size.y;
  auto upc = adjust_aspect(pc, aspect);
  auto wnd = casein::mouse_pos / casein::window_size;
  auto rel = wnd * 2.0 - 1.0f;
  return rel * upc.grid_size + upc.grid_pos;
}
}
