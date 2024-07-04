#version 450

layout(push_constant) uniform upc {
  vec2 grid_pos;
  vec2 grid_size;
} pc;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 i_pos;
layout(location = 2) in vec4 i_color;
layout(location = 3) in vec4 i_uv;
layout(location = 4) in vec4 i_mult;
layout(location = 5) in vec4 i_rot;

layout(location = 0) out vec4 q_color;
layout(location = 1) out vec2 q_uv;
layout(location = 2) out vec4 q_mult;

const float pi = 3.14159265358979323f;

void main() {
  q_color = i_color;
  q_uv = mix(i_uv.xy, i_uv.zw, pos);
  q_mult = i_mult;

  vec2 f_adj = pos * 0.0001f; // avoid one-pixel gaps

  float theta = i_rot.x * pi / 180.0f;
  const mat3 rot = mat3(
    cos(theta), -sin(theta), 0,
    sin(theta), cos(theta), 0,
    0, 0, 1
  );

  vec3 pp = rot * vec3(pos * i_pos.zw, 1);

  vec2 f_pos = (pp.xy + i_pos.xy - pc.grid_pos) / pc.grid_size; 
  vec3 f_pos3 = rot * vec3(f_pos, 1);
  gl_Position = vec4(f_pos + f_adj, 0, 1);
}
