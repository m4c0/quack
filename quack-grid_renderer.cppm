export module quack:grid_renderer;
import :objects;
import :ilayout;
import casein;

namespace quack {
export template <auto W, auto H, typename Tp>
class grid_ilayout : public instance_layout<Tp, W * H> {
  using parent_t = instance_layout<Tp, W * H>;

  void setup() override {
    parent_t::setup();

    this->batch()->set_count(cells);
    this->batch()->positions().map([](rect *is) {
      unsigned i = 0;
      for (float y = 0; y < H; y++) {
        for (float x = 0; x < W; x++, i++) {
          is[i] = {x, y, 1, 1};
        }
      }
    });
  };
  void resize(unsigned w, unsigned h) override {
    this->batch()->resize(W, H, w, h);
  }

public:
  static constexpr const auto width = W;
  static constexpr const auto height = H;
  static constexpr const auto cells = width * height;

  using parent_t::at;
  using parent_t::parent_t;

  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) noexcept {
    return parent_t::at(y * W + x);
  }
  [[nodiscard]] constexpr const auto &at(unsigned x,
                                         unsigned y) const noexcept {
    return parent_t::at(y * W + x);
  }
};
} // namespace quack
