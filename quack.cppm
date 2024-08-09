export module quack;
export import :donald;
export import :objects;
export import :upc;

#ifdef LECO_TARGET_WASM
#pragma leco add_impl donald_wasm
#else
export import :pipeline_stuff;
import voo;

namespace quack {
  export class buffer_updater : public voo::updater<voo::h2l_buffer> {
    void (*m_fn)(instance *) {};

    virtual void update_data(voo::h2l_buffer * buf) override;

  public:
    buffer_updater(voo::device_and_queue * dq, unsigned max_quads, void (*fn)(instance *));
  };
} // namespace quack

#pragma leco add_shader "quack.frag"
#pragma leco add_shader "quack.vert"
#pragma leco add_impl donald updater
#endif
