export module quack:pipeline_stuff;
import :objects;
import :bbuffer;
import :per_device;
import :per_extent;
import missingno;
import vee;

namespace quack {
struct upc {
  pos grid_pos{};
  pos grid_size{};
};

class pipeline_stuff {
  const per_device *dev;

  vee::descriptor_set_layout dsl =
      vee::create_descriptor_set_layout({vee::dsl_fragment_sampler()});

  vee::pipeline_layout pl = vee::create_pipeline_layout(
      {*dsl}, {vee::vert_frag_push_constant_range<upc>()});

  vee::shader_module vert =
      vee::create_shader_module_from_resource("quack.vert.spv");
  vee::shader_module frag =
      vee::create_shader_module_from_resource("quack.frag.spv");

  vee::descriptor_pool desc_pool =
      vee::create_descriptor_pool(1, {vee::combined_image_sampler()});
  vee::descriptor_set desc_set = vee::allocate_descriptor_set(*desc_pool, *dsl);
  vee::sampler smp = vee::create_sampler(vee::nearest_sampler);

  static constexpr const auto v_count = 6;

  bound_buffer<pos> vertices{bb_vertex{}, dev, v_count};
  bound_buffer<pos> instance_pos;
  bound_buffer<colour> instance_colour;
  bound_buffer<uv> instance_uv;
  upc pc;
  unsigned m_max_quads;

  pos m_mouse_pos{};
  pos m_screen_disp{};
  pos m_screen_scale{};

  void map_vertices() {
    vertices.map([](auto vs) {
      vs[0] = {0, 0};
      vs[1] = {1, 1};
      vs[2] = {1, 0};

      vs[3] = {1, 1};
      vs[4] = {0, 0};
      vs[5] = {0, 1};
    });
  }

public:
  pipeline_stuff(const per_device *d, unsigned max_quads)
      : dev{d}, instance_pos{bb_vertex{}, dev, max_quads},
        instance_colour{bb_vertex{}, dev, max_quads},
        instance_uv{bb_vertex{}, dev, max_quads}, m_max_quads{max_quads} {
    map_vertices();
  }

  void resize(const params &p, float sw, float sh) {
    float aspect = sw / sh;
    float gw = p.grid_w / 2.0;
    float gh = p.grid_h / 2.0;
    float grid_aspect = gw / gh;
    pc.grid_pos = pos{gw, gh};
    pc.grid_size =
        grid_aspect < aspect ? pos{aspect * gh, gh} : pos{gw, gw / aspect};
    m_screen_scale = {2.0f * pc.grid_size.x / sw, 2.0f * pc.grid_size.y / sh};
    m_screen_disp = {pc.grid_size.x - gw, pc.grid_size.y - gh};
  }

  mno::opt<unsigned> current_hover() {
    mno::opt<unsigned> res{};
    instance_pos.map([&](auto *is) {
      for (auto i = 0U; i < m_max_quads; i++) {
        if (m_mouse_pos.x < is[i].x)
          continue;
        if (m_mouse_pos.y < is[i].y)
          continue;
        if (m_mouse_pos.x > is[i].x + 1.0f)
          continue;
        if (m_mouse_pos.y > is[i].y + 1.0f)
          continue;
        res = i;
        break;
      }
    });
    return res;
  }
  void mouse_move(float x, float y) {
    m_mouse_pos.x = x * m_screen_scale.x - m_screen_disp.x;
    m_mouse_pos.y = y * m_screen_scale.y - m_screen_disp.y;
  }

  void set_atlas(const vee::image_view &iv) {
    vee::update_descriptor_set(desc_set, 0, *iv, *smp);
  }

  void map_instances_pos(const filler<pos> &fn) { instance_pos.map(fn); }
  void map_instances_colour(const filler<colour> &fn) {
    instance_colour.map(fn);
  }
  void map_instances_uv(const filler<uv> &fn) { instance_uv.map(fn); }

  void build_commands(vee::command_buffer cb, unsigned i_count) const {
    vee::cmd_bind_vertex_buffers(cb, 0, *vertices);
    vee::cmd_bind_vertex_buffers(cb, 1, *instance_pos);
    vee::cmd_bind_vertex_buffers(cb, 2, *instance_colour);
    vee::cmd_bind_vertex_buffers(cb, 3, *instance_uv);
    vee::cmd_bind_descriptor_set(cb, *pl, 0, desc_set);
    vee::cmd_push_vert_frag_constants(cb, *pl, &pc);
    vee::cmd_draw(cb, v_count, i_count);
  }

  [[nodiscard]] auto create_pipeline(const per_extent *ext) const {
    return vee::create_graphics_pipeline(
        *pl, ext->render_pass(),
        {
            vee::pipeline_vert_stage(*vert, "main"),
            vee::pipeline_frag_stage(*frag, "main"),
        },
        {
            vee::vertex_input_bind(sizeof(pos)),
            vee::vertex_input_bind_per_instance(sizeof(pos)),
            vee::vertex_input_bind_per_instance(sizeof(colour)),
            vee::vertex_input_bind_per_instance(sizeof(uv)),
        },
        {
            vee::vertex_attribute_vec2(0, 0),
            vee::vertex_attribute_vec2(1, 0),
            vee::vertex_attribute_vec4(2, 0),
            vee::vertex_attribute_vec4(3, 0),
        });
  }
};
} // namespace quack
