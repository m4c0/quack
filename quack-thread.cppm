export module quack:thread;
import :agg;
import :objects;
import :per_device;
import :per_extent;
import :per_frame;
import :per_inflight;
import :pipeline;
import :pipeline_stuff;
import :stage;
import casein;
import sith;
import vee;

namespace quack {
struct resized_exception {
  const params &p;
  float sw;
  float sh;
};
class thread : public sith::thread {
  casein::native_handle_t m_nptr;
  unsigned m_max_quads;

  void run() override {
    vee::initialise();
    level_0 l0{m_nptr, m_max_quads};

    while (!interrupted()) {
      try {
        vee::device_wait_idle();
        level_1 l1{&l0};

        while (!interrupted()) {
        }
      } catch (const resized_exception &e) {
        // ps.resize(e.p, e.sw, e.sh);
      }
    }
  }

public:
  explicit thread(casein::native_handle_t nptr, unsigned max_quads)
      : sith::thread{false}, m_nptr{nptr}, m_max_quads{max_quads} {}
};
} // namespace quack
