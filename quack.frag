#version 450

layout(push_constant) uniform upc {
  vec2 grid_pos;
  vec2 grid_size;
  vec2 mouse_pos;
} pc;

layout(set = 0, binding = 0) uniform sampler2D tex;
//layout(set = 0, binding = 2) buffer writeonly pick {
//  float mouse_distance[];
//};

layout(location = 0) in vec4 i_color;
layout(location = 1) in vec2 i_uv;

layout(location = 0) out vec4 frag_color;

void main() {
  float mouse_distance = length(pc.mouse_pos - gl_FragCoord.xy);
  vec4 tex_color = texture(tex, i_uv);
  vec4 mix_color = mix(i_color, tex_color, tex_color.a);
  frag_color = mix_color + vec4(0, 0, mouse_distance / 256.0, 0);
}
