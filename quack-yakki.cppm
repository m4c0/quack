export module quack:yakki;
import :objects;
import :upc;
import :updater;
import dotz;
import jute;
import sith;
import traits;

export namespace quack::yakki {
  class buffer {
    buffer_updater m_buffer {};
    upc m_pc {};
    sith::run_guard m_guard {};

  public:
    constexpr buffer() = default;
    buffer(buffer_updater b) : m_buffer { traits::move(b) } {}

    [[nodiscard]] constexpr auto & pc() { return m_pc; }

    [[nodiscard]] constexpr auto local_buffer() const { return m_buffer.data().local_buffer(); }
    [[nodiscard]] constexpr auto count() const { return m_buffer.count(); }

    [[nodiscard]] dotz::vec2 mouse_pos() const;

    void start() { m_guard = sith::run_guard { &m_buffer }; }
    void run_once() { m_buffer.run_once(); }
  };
  using image = image_updater;

  struct resources {
    [[nodiscard]] virtual image * image(jute::view name) = 0;
    [[nodiscard]] virtual buffer * buffer(unsigned size, buffer_fn_t && fn) = 0;
  };

  struct renderer {
    virtual void run(buffer * b, image * i, unsigned count, unsigned first = 0) = 0;
    virtual void run(buffer * b, image * i) = 0;
  };

  extern void (*on_start)(resources *);
  extern void (*on_frame)(renderer *);
  extern dotz::vec4 clear_colour;
} // namespace quack::yakki
