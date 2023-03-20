export module quack:objects;

namespace quack {
export struct params {
  unsigned grid_w;
  unsigned grid_h;
  unsigned max_quads;
};

export struct pos {
  float x;
  float y;
};
export struct colour {
  float r;
  float g;
  float b;
  float a; // Currently unused
};
template <typename Tp> struct filler {
  virtual void operator()(Tp *) const noexcept = 0;
};
} // namespace quack
