import casein;
import quack;

extern "C" void casein_handle(const casein::event &e) {
  static quack::renderer r{3};
  static quack::instance_layout<quack::rect, 2> s{&r};
  static quack::grid_ilayout<30, 20, quack::colour> q{&r};
  static quack::grid_ilayout<10, 10, quack::colour> p{&r};

  r.process_event(e);
  s.process_event(e);
  q.process_event(e);
  p.process_event(e);

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.load_atlas(16, 32, [](auto *img) {
      for (auto i = 0; i < 16 * 16; i++) {
        img[i] = {255, 255, 255, 0};
        img[i + 256] = {255, 255, 255, 128};
      }
    });

    s.reset_grid();
    s.at(0) = quack::rect{0, 0, 1, 1};
    s.at(1) = quack::rect{0.25, 0.25, 0.5, 0.5};
    s.fill_pos([](auto p) { return p; });
    s.fill_colour([](auto p) { return quack::colour{p.x, 0, 0.1, 1}; });
    s.fill_uv([](auto) { return quack::uv{}; });
    s.batch()->resize(1, 1, 1, 1);

    q.reset_grid();
    q.at(9, 10) = quack::colour{0, 0, 0, 1};
    q.at(10, 10) = quack::colour{1, 0, 0, 1};
    q.at(12, 8) = quack::colour{0, 0, 1, 1};
    q.at(14, 10) = quack::colour{0, 1, 0, 1};
    q.fill_colour([](quack::colour qs) { return qs; });
    q.fill_uv([](quack::colour qs) { return quack::uv{{0, 0}, {1, 0.5}}; });

    p.reset_grid();
    p.at(0, 0) = quack::colour{1, 1, 0, 1};
    p.at(9, 9) = quack::colour{0, 1, 1, 1};
    p.fill_colour([](quack::colour qs) { return qs; });
    p.fill_uv([](quack::colour qs) { return quack::uv{{0, 0}, {1, 0.5}}; });
    break;
  case casein::MOUSE_DOWN:
    q.current_hover().consume([&](auto idx) {
      q.at(idx) = quack::colour{1, 1, 1, 1};
    });
    q.fill_colour([](quack::colour qs) { return qs; });
  default:
    break;
  }
}
