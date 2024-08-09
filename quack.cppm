export module quack;
export import :donald;
export import :objects;
export import :upc;

#ifdef LECO_TARGET_WASM
#pragma leco add_impl donald_wasm
#else
export import :pipeline_stuff;
import vee;
import voo;

namespace quack {
  export class buffer_updater : public voo::updater<voo::h2l_buffer> {
    void (*m_fn)(instance *) {};

    void update_data(voo::h2l_buffer * buf) override;

  public:
    buffer_updater(voo::device_and_queue * dq, unsigned max_quads, void (*fn)(instance *));
  };

  export class image_updater : public voo::updater<voo::h2l_image> {
    voo::h2l_image (*m_fn)(vee::physical_device) {};

    vee::sampler m_smp = vee::create_sampler(vee::nearest_sampler);
    vee::physical_device m_pd;
    vee::descriptor_set m_dset;

    voo::h2l_image m_old {};
    vee::descriptor_set m_dset_old;

    void update_data(voo::h2l_image * buf) override;

  public:
    image_updater(voo::device_and_queue * dq, pipeline_stuff * ps, decltype(m_fn) fn);

    [[nodiscard]] constexpr const auto dset() const { return m_dset; }
  };
} // namespace quack

#pragma leco add_shader "quack.frag"
#pragma leco add_shader "quack.vert"
#pragma leco add_impl donald updater
#endif
