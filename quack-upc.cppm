export module quack:upc;
import dotz;

namespace quack {
export struct upc {
  dotz::vec2 grid_pos{};
  dotz::vec2 grid_size{1, 1};
};
export [[nodiscard]] constexpr upc adjust_aspect(upc u, float aspect) {
  auto g = u.grid_size / 2.0;
  float grid_aspect = g.x / g.y;
  u.grid_size = grid_aspect < aspect ? dotz::vec2{aspect * g.y, g.y}
                                     : dotz::vec2{g.x, g.x / aspect};
  return u;
}
} // namespace quack
