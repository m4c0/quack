import casein;
import quack;

extern "C" void casein_handle(const casein::event &e) {
  static quack::grid_renderer<30, 20, quack::colour> q;
  q.process_event(e);

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    q.load_atlas(16, 16, [](auto *img) {
      for (auto i = 0; i < 16 * 16; i++) {
        img[i] = {255, 255, 255, 127};
      }
    });
    q.reset_grid();
    q.at(10, 10) = quack::colour{1, 0, 0, 1};
    q.at(12, 8) = quack::colour{0, 0, 1, 1};
    q.at(14, 10) = quack::colour{0, 1, 0, 1};
    q.fill_colour([](quack::colour qs) { return qs; });
    q.fill_uv([](quack::colour qs) { return quack::uv{{0, 0}, {1, 1}}; });
    break;
  default:
    break;
  }
}
