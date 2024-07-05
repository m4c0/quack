export module quack:objects;
import dotz;

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

export struct u8_rgba {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};
} // namespace quack
