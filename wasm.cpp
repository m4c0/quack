module quack;
import :objects;
import casein;
import hai;

extern "C" void quack_fill_colour(float r, float g, float b);
extern "C" void quack_fill_rect(unsigned x, unsigned y, unsigned w, unsigned h);
extern "C" void quack_load_atlas(unsigned w, unsigned h, void *buf);

namespace quack {
class wpimpl : public pimpl {
  hai::holder<colour[]> m_colour;
  hai::holder<pos[]> m_pos;
  hai::holder<uv[]> m_uvs;
  unsigned m_grid_w;
  unsigned m_grid_h;
  unsigned m_canvas_w;
  unsigned m_canvas_h;

public:
  explicit wpimpl(const params &p)
      : m_colour{hai::holder<colour[]>::make(p.max_quads)},
        m_pos{hai::holder<pos[]>::make(p.max_quads)},
        m_uvs{hai::holder<uv[]>::make(p.max_quads)}, m_grid_w{p.grid_w},
        m_grid_h{p.grid_h} {}

  void fill_colour(const filler<colour> &g) override { g(*m_colour); }
  void fill_pos(const filler<pos> &g) override { g(*m_pos); }
  void fill_uv(const filler<uv> &g) override { g(*m_uvs); }

  void load_atlas(unsigned w, unsigned h, const filler<u8_rgba> &g) override {
    auto tmp = hai::holder<u8_rgba[]>::make(w * h);
    g(*tmp);
    quack_load_atlas(w, h, *tmp);
  }

  void repaint(unsigned i_count) override {
    for (auto i = 0; i < i_count; i++) {
      const auto &b = (*m_colour)[i];
      quack_fill_colour(b.r, b.g, b.b);

      const auto &p = (*m_pos)[i];
      const auto w = m_canvas_w / m_grid_w;
      const auto h = m_canvas_h / m_grid_h;
      const auto x = w * p.x;
      const auto y = h * p.y;
      quack_fill_rect(x, y, w, h);
    }
  }

  void resize(unsigned w, unsigned h, float) override {
    m_canvas_w = w;
    m_canvas_h = h;
  }

  void setup(casein::native_handle_t) override {}
};

renderer::renderer(const params &p)
    : m_pimpl{hai::uptr<pimpl>(new wpimpl(p))} {}
} // namespace quack
