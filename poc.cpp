import casein;
import quack;

struct filler : quack::filler<quack::colour>, quack::filler<quack::pos> {
  void operator()(quack::colour *qs) const noexcept override {
    qs[0] = {1, 1, 1, 1};
    qs[1] = {1, 0, 1, 1};
  }
  void operator()(quack::pos *qs) const noexcept override {
    qs[0] = {-1, -1};
    qs[1] = {1, 1};
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static constexpr const quack::params p{
      .grid_w = 3,
      .grid_h = 2,
      .max_quads = 6,
  };
  static quack::renderer q{p};
  switch (e.type()) {
  case casein::CREATE_WINDOW:
    q.setup(e.as<casein::events::create_window>().native_window_handle());
    q.fill_pos(filler{});
    break;
  case casein::REPAINT:
    q.fill_colour(filler{});
    q.repaint(2);
    break;
  case casein::QUIT:
    q.quit();
    break;
  default:
    break;
  }
}
