export module quack:objects;

export namespace quack {
struct quad {
  float r;
  float g;
  float b;
  float a; // Currently unused
};
struct filler {
  virtual void operator()(quad *) const noexcept = 0;
};
} // namespace quack
