import casein;
import quack;

struct filler : quack::filler<quack::colour>, quack::filler<quack::pos> {
  void operator()(quack::colour *qs) const noexcept override {}
  void operator()(quack::pos *qs) const noexcept override {}
};

extern "C" void casein_handle(const casein::event &e) {
  static constexpr const auto max_quads = 16;
  static quack::renderer q{max_quads};
  switch (e.type()) {
  case casein::CREATE_WINDOW:
    q.setup(e.as<casein::events::create_window>().native_window_handle());
    q.fill_pos(filler{});
    break;
  case casein::REPAINT:
    q.fill_colour(filler{});
    q.repaint();
    break;
  case casein::QUIT:
    q.quit();
    break;
  default:
    break;
  }
}
