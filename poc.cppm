#pragma leco app
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

  r.process_event(e);
  s.process_event(e);
  quack::mouse_tracker::instance().handle(e);

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    s->load_atlas(16, 32, atlas_image);

    // Background + Pink rect
    s->map_positions([](auto *ps) { ps[0] = {{0, 0}, {1, 1}}; });
    s->map_colours([](auto *cs) { cs[0] = {0, 0, 0.1, 1.0}; });
    s->map_uvs([](auto *us) { us[0] = {}; });
    s->map_multipliers([](auto *ms) { ms[0] = {1, 1, 1, 1}; });
    s->map_all([](auto p) {
      auto &[cs, ms, ps, us] = p;
      ps[1] = {{0.25, 0.25}, {0.5, 0.5}};
      cs[1] = {0.25, 0, 0.1, 1.0};
      us[1] = {};
      ms[1] = {1, 1, 1, 1};
    });
    s->center_at(0.5, 0.5);
    s->set_count(2);
    s->set_grid(1, 1);
    break;
  default:
    break;
  }
}
