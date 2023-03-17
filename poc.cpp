import casein;
import quack;

struct filler : public quack::filler {
  void operator()(quack::quad *qs) const noexcept override {}
};

extern "C" void casein_handle(const casein::event &e) {
  static constexpr const auto max_quads = 16;
  static quack::renderer q{max_quads};
  switch (e.type()) {
  case casein::CREATE_WINDOW:
    q.setup(e.as<casein::events::create_window>().native_window_handle());
    break;
  case casein::REPAINT:
    q.update(filler{});
    q.repaint();
    break;
  case casein::QUIT:
    q.quit();
    break;
  default:
    break;
  }
}
