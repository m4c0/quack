export module quack;
export import :donald;
export import :objects;
export import :upc;

#ifdef LECO_TARGET_WASM
#pragma leco add_impl donald_wasm
#else
export import :daffy;
export import :pipeline_stuff;
import hai;
import vee;
import voo;

namespace quack {
  export class buffer_updater : public voo::updater<voo::h2l_buffer> {
    buffer_fn_t m_fn {};
    unsigned m_inst_count {};

    void update_data(voo::h2l_buffer * buf) override;

  public:
    constexpr buffer_updater() = default;
    buffer_updater(voo::device_and_queue * dq, unsigned max_quads, buffer_fn_t fn);

    [[nodiscard]] constexpr auto count() const { return m_inst_count; }
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

    [[nodiscard]] constexpr const auto & smp() const { return m_smp; }
    [[nodiscard]] constexpr auto & smp() { return m_smp; }

    [[nodiscard]] constexpr const auto dset() const { return m_dset; }
  };
} // namespace quack

#pragma leco add_shader "quack.frag"
#pragma leco add_shader "quack.vert"
#pragma leco add_impl daffy donald updater
#endif
