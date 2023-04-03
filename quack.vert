#version 450

layout(push_constant) uniform upc {
  vec2 grid_pos;
  vec2 grid_size;
  vec2 mouse_pos;
} pc;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 i_pos;
layout(location = 2) in vec4 i_color;
layout(location = 3) in vec4 i_uv;

layout(location = 0) out vec4 q_color;
layout(location = 1) out vec2 q_uv;

void main() {
  vec2 f_pos = (pos + i_pos - pc.grid_pos) / pc.grid_size; 
  q_color = i_color;
  q_uv = mix(i_uv.xy, i_uv.zw, pos);
  gl_Position = vec4(f_pos, 0, 1);
}
