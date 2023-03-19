export module quack:objects;

export namespace quack {
struct pos {
  float x;
  float y;
};
struct colour {
  float r;
  float g;
  float b;
  float a; // Currently unused
};
template <typename Tp> struct filler {
  virtual void operator()(Tp *) const noexcept = 0;
};
} // namespace quack
