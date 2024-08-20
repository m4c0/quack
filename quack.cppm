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

#pragma leco add_shader "quack.frag"
#pragma leco add_shader "quack.vert"
#pragma leco add_impl donald updater yakki
#endif
