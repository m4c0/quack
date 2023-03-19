export module quack:v_bbuffer;
import :v_per_device;
import vee;

namespace quack {
template <typename Tp> class bound_buffer {
  const per_device *m_dev;

  vee::buffer m_buf;
  vee::device_memory m_mem =
      vee::create_host_buffer_memory(m_dev->physical_device(), *m_buf);
  decltype(nullptr) m_bind = vee::bind_buffer_memory(*m_buf, *m_mem);

public:
  explicit bound_buffer(const per_device *d, unsigned max)
      : m_dev{d}, m_buf{vee::create_vertex_buffer(sizeof(Tp) * max)} {}

  [[nodiscard]] auto operator*() const noexcept { return *m_buf; }

  void map(auto &&fn) { vee::map_memory<Tp>(*m_mem, fn); }
};
} // namespace quack
