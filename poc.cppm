export module poc;

import casein;
import quack;

void atlas_image(quack::u8_rgba *img) {
  for (auto i = 0; i < 16 * 16; i++) {
    auto x = (i / 16) % 2;
    auto y = (i % 16) % 2;
    unsigned char b = (x ^ y) == 0 ? 255 : 0;

    img[i] = {255, 255, 255, 0};
    img[i + 256] = {b, b, b, 128};
  }
}

extern "C" void casein_handle(const casein::event &e) {
  static quack::renderer r{3};
  static quack::ilayout s{&r, 2};
  static quack::grid_ilayout<30, 20, quack::colour> q{&r};
  static quack::grid_ilayout<10, 10, quack::colour> p{&r};
  static quack::mouse_tracker mouse{};

  r.process_event(e);
  s.process_event(e);
  q.process_event(e);
  p.process_event(e);
  mouse.process_event(e);

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    s.batch()->load_atlas(16, 32, atlas_image);
    q.batch()->load_atlas(16, 32, atlas_image);
    p.batch()->load_atlas(16, 32, atlas_image);

    // Background + Pink rect
    s.batch()->map_positions([](auto *ps) {
      ps[0] = {0, 0, 1, 1};
      ps[1] = {0.25, 0.25, 0.5, 0.5};
    });
    s.batch()->map_colours([](auto *cs) {
      cs[0] = {0, 0, 0.1, 1.0};
      cs[1] = {0.25, 0, 0.1, 1.0};
    });
    s.batch()->map_uvs([](auto *us) { us[0] = us[1] = {}; });
    s.batch()->map_multipliers([](auto *ms) { ms[0] = ms[1] = {1, 1, 1, 1}; });
    s.batch()->center_at(0.5, 0.5);
    s.batch()->set_count(2);
    s.set_grid(1, 1);

    // Togglable quads
    q.reset_grid();
    q.at(9, 10) = quack::colour{0, 0, 0, 1};
    q.at(10, 10) = quack::colour{1, 0, 0, 1};
    q.at(12, 8) = quack::colour{0, 0, 1, 1};
    q.at(14, 10) = quack::colour{0, 1, 0, 1};
    q.fill_colour([](quack::colour qs) { return qs; });
    q.fill_uv([](quack::colour qs) { return quack::uv{{0, 0}, {1, 0.5}}; });
    q.fill_mult([](auto p) { return quack::colour{1, 1, 1, 1}; });

    // Quads aligned with corners
    p.reset_grid();
    p.at(0, 0) = quack::colour{1, 1, 0, 1};
    p.at(9, 9) = quack::colour{0, 1, 1, 1};
    p.fill_colour([](quack::colour qs) { return qs; });
    p.fill_uv([](quack::colour qs) { return quack::uv{{0, 0.5}, {1, 1}}; });
    p.fill_mult([](quack::colour qs) { return qs; });
    break;
  case casein::MOUSE_DOWN:
    mouse.current_hover(q.batch()).consume([&](auto idx) {
      q.at(idx) = quack::colour{1, 1, 1, 1};
    });
    q.fill_colour([](quack::colour qs) { return qs; });
  default:
    break;
  }
}
