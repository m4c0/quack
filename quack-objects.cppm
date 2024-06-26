export module quack:objects;
import dotz;

namespace quack {
export struct pos {
  float x;
  float y;
};
export struct size {
  float w;
  float h;
};
export struct rect : pos, size {};
static_assert(sizeof(rect) == 4 * sizeof(float));

export struct uv {
  dotz::vec2 start;
  dotz::vec2 end;
};
export struct colour {
  float r;
  float g;
  float b;
  float a; // Currently unused
};
export struct u8_rgba {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};
} // namespace quack
