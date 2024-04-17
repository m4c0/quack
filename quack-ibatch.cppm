export module quack:ibatch;
import :extent;
import :mouse;
import :objects;
import dotz;
import jute;
import silog;
import traits;
import vee;
import voo;

namespace quack {
export class instance_batch {
  voo::h2l_buffer m_pos;
  voo::h2l_buffer m_colour;
  voo::h2l_buffer m_mult;
  voo::h2l_buffer m_uv;

  template <typename Tp>
  static auto create_buf(vee::physical_device pd, unsigned max_quads) {
    auto sz = static_cast<unsigned>(max_quads * sizeof(Tp));
    return voo::h2l_buffer{pd, sz};
  }

public:
  instance_batch() = default;
  instance_batch(vee::physical_device pd, vee::pipeline_layout::type pl,
                 unsigned max_quads)
      : m_pos{create_buf<rect>(pd, max_quads)},
        m_colour{create_buf<colour>(pd, max_quads)},
        m_mult{create_buf<colour>(pd, max_quads)},
        m_uv{create_buf<uv>(pd, max_quads)} {}

  void map_colours(auto &&fn) noexcept {
    voo::mapmem m{m_colour.host_memory()};
    fn(static_cast<colour *>(*m));
  }
  void map_multipliers(auto &&fn) noexcept {
    voo::mapmem m{m_mult.host_memory()};
    fn(static_cast<colour *>(*m));
  }
  void map_positions(auto &&fn) noexcept {
    voo::mapmem m{m_pos.host_memory()};
    fn(static_cast<rect *>(*m));
  }
  void map_uvs(auto &&fn) noexcept {
    voo::mapmem m{m_uv.host_memory()};
    fn(static_cast<uv *>(*m));
  }
  void map_all(auto &&fn) noexcept {
    struct {
      colour *colours;
      colour *multipliers;
      rect *positions;
      uv *uvs;
    } all;

    voo::mapmem c{m_colour.host_memory()};
    voo::mapmem m{m_mult.host_memory()};
    voo::mapmem p{m_pos.host_memory()};
    voo::mapmem u{m_uv.host_memory()};
    all.colours = static_cast<colour *>(*c);
    all.multipliers = static_cast<colour *>(*m);
    all.positions = static_cast<rect *>(*p);
    all.uvs = static_cast<uv *>(*u);

    fn(all);
  }

  void setup_copy(vee::command_buffer cb) const {
    m_colour.setup_copy(cb);
    m_mult.setup_copy(cb);
    m_pos.setup_copy(cb);
    m_uv.setup_copy(cb);
  }

  void build_commands(vee::command_buffer cb) const {
    vee::cmd_bind_vertex_buffers(cb, 1, m_pos.local_buffer());
    vee::cmd_bind_vertex_buffers(cb, 2, m_colour.local_buffer());
    vee::cmd_bind_vertex_buffers(cb, 3, m_uv.local_buffer());
    vee::cmd_bind_vertex_buffers(cb, 4, m_mult.local_buffer());
  }
};

export class instance_batch_thread : public voo::update_thread {
  instance_batch m_ib;

protected:
  instance_batch_thread(voo::queue *q, instance_batch ib)
      : update_thread{q}
      , m_ib{traits::move(ib)} {}

  void build_cmd_buf(vee::command_buffer cb) override {
    map_data(&m_ib);

    voo::cmd_buf_one_time_submit pcb{cb};
    m_ib.setup_copy(cb);
  }

  virtual void map_data(instance_batch *ib) = 0;

public:
  [[nodiscard]] constexpr auto &batch() noexcept { return m_ib; }
};
} // namespace quack
