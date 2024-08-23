export module quack:objects;
import dotz;
import hai;

namespace quack {
export struct rotation {
  float angle;
  float rel_x;
  float rel_y;
  float pad; // Currently unused
};

export struct instance {
  dotz::vec2 position;
  dotz::vec2 size;
  dotz::vec2 uv0;
  dotz::vec2 uv1;
  dotz::vec4 colour;
  dotz::vec4 multiplier;
  rotation rotation;
};
static_assert(sizeof(instance) == 20 * sizeof(float));

export struct upc {
  dotz::vec2 grid_pos{};
  dotz::vec2 grid_size{1, 1};
};

export struct scissor {
  dotz::vec2 offset;
  dotz::vec2 extent;
  const upc * ref;
};

using buffer_fn_t = hai::fn<void, instance *&>;
} // namespace quack
