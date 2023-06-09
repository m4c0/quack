export module quack:objects;

namespace quack {
static constexpr const auto v_count = 6;

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
  struct {
    float u;
    float v;
  } start;
  struct {
    float u;
    float v;
  } end;
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
export template <typename Tp> struct filler {
  virtual void operator()(Tp *) const noexcept = 0;
};

struct upc {
  pos grid_pos{};
  size grid_size{};
};
} // namespace quack
