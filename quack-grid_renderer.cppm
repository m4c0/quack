export module quack:grid_renderer;
import :objects;
import :ilayout;
import :renderer;
import casein;

namespace quack {
export template <auto W, auto H, typename Tp> class grid_ilayout {
  ilayout m_il;
  Tp m_data[W * H];

  void setup() {
    set_grid(W, H);
    this->batch()->center_at(W / 2, H / 2);
    this->batch()->set_count(cells);
    this->batch()->map_positions([](rect *is) {
      unsigned i = 0;
      for (float y = 0; y < H; y++) {
        for (float x = 0; x < W; x++, i++) {
          is[i] = {x, y, 1, 1};
        }
      }
    });
  };

public:
  static constexpr const auto width = W;
  static constexpr const auto height = H;
  static constexpr const auto cells = width * height;

  explicit constexpr grid_ilayout(renderer *r) : m_il{r, W * H} {}

  [[nodiscard]] constexpr auto &at(unsigned idx) noexcept {
    return m_data[idx];
  }
  [[nodiscard]] constexpr const auto &at(unsigned idx) const noexcept {
    return m_data[idx];
  }
  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) noexcept {
    return at(y * W + x);
  }
  [[nodiscard]] constexpr const auto &at(unsigned x,
                                         unsigned y) const noexcept {
    return at(y * W + x);
  }

  [[nodiscard]] constexpr auto *batch() noexcept { return m_il.batch(); }

  void process_event(const casein::event &e) {
    m_il.process_event(e);

    if (e.type() == casein::CREATE_WINDOW)
      setup();
  }

  constexpr void set_grid(unsigned gw, unsigned gh) noexcept {
    m_il.set_grid(gw, gh);
  }

  void fill_colour(auto &&fn) {
    this->batch()->map_colours([&](auto *c) {
      for (auto i = 0; i < W * H; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_pos(auto &&fn) {
    this->batch()->map_positions([&](auto *c) {
      for (auto i = 0; i < W * H; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_mult(auto &&fn) {
    this->batch()->map_multipliers([&](auto *c) {
      for (auto i = 0; i < W * H; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void fill_uv(auto &&fn) {
    this->batch()->map_uvs([&](auto *c) {
      for (auto i = 0; i < W * H; i++) {
        c[i] = fn(at(i));
      }
    });
  }
  void reset_grid() {
    for (auto &d : m_data)
      d = {};
  }

  [[nodiscard]] constexpr auto &data() noexcept { return m_data; }
};
} // namespace quack
