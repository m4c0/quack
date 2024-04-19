#pragma leco app
#pragma leco add_resource "nasa-jupiter.png"
import casein;
import quack;
import voo;

struct : public quack::donald {
  const char *app_name() const noexcept override { return "quack-donald-poc"; }
  unsigned max_quads() const noexcept override { return 16; }

  quack::upc push_constants() const noexcept override {
    return {
        .grid_pos = {2, 2},
        .grid_size = {4, 4},
    };
  }

  void update_data(quack::mapped_buffers all) override {
    auto [c, m, p, u] = all;
    for (auto y = 0, i = 0; y < 4; y++) {
      for (auto x = 0; x < 4; x++, i++) {
        float xf = x + 0.05;
        float yf = y + 0.05;
        c[i] = {1, 1, 1, 1};
        m[i] = {1, 1, 1, 1};
        p[i] = {{xf, yf}, {0.9, 0.9}};
        u[i] = {{0, 0}, {1, 1}};
      }
    }
  }

  atlas create_atlas(voo::device_and_queue *dq) override {
    return atlas{new voo::sires_image{"nasa-jupiter.png", dq}};
  }
} d;
