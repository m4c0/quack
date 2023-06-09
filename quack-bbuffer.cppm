export module quack:bbuffer;
import :per_device;
import traits;
import vee;

namespace quack {
struct bb_storage {};
struct bb_vertex {};
template <typename Tp> class bound_buffer {
  const per_device *m_dev;

  vee::buffer m_buf;
  vee::device_memory m_mem =
      vee::create_host_buffer_memory(m_dev->physical_device(), *m_buf);
  decltype(nullptr) m_bind = vee::bind_buffer_memory(*m_buf, *m_mem);

public:
  bound_buffer(bb_storage, const per_device *d, unsigned max)
      : m_dev{d}, m_buf{vee::create_storage_buffer(sizeof(Tp) * max)} {}
  bound_buffer(bb_vertex, const per_device *d, unsigned max)
      : m_dev{d}, m_buf{vee::create_vertex_buffer(sizeof(Tp) * max)} {}

  [[nodiscard]] auto operator*() const noexcept { return *m_buf; }
  [[nodiscard]] auto map() const { return vee::mapmem(*m_mem); }
};
} // namespace quack
