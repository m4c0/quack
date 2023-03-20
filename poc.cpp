import casein;
import quack;

extern "C" void casein_handle(const casein::event &e) {
  static quack::grid_renderer<30, 20, quack::colour> q;
  switch (e.type()) {
  case casein::CREATE_WINDOW:
    q.setup(e.as<casein::events::create_window>().native_window_handle());
    q.at(10, 10) = quack::colour{1, 0, 0, 1};
    q.at(12, 8) = quack::colour{0, 0, 1, 1};
    q.at(14, 10) = quack::colour{0, 1, 0, 1};
    break;
  case casein::REPAINT:
    q.fill_colour([](quack::colour qs) { return qs; });
    q.repaint();
    break;
  case casein::QUIT:
    q.quit();
    break;
  default:
    break;
  }
}
