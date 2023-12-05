#version 430 core
layout(location = 0) in vec3 pos;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;
uniform uint frame;
uniform sampler2D height_map;
uniform float WAVE_SIZE;
uniform bool use_height_map;

out vec3 vs_world_pos;


void main() {
  if (use_height_map) {
    vec2 TexCoord = clamp((vec2(pos.x , pos.z) + WAVE_SIZE) / (2.f * WAVE_SIZE), 0, 1);

    vec4 info = texture2D(height_map, TexCoord);
    vs_world_pos = vec3(pos.x, (info.r * 0.5 - 0.5), pos.z);
    gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);
  }
  else {
    // changes over frames
    float offset = float(frame) / 20;

    // y  = 0.05 * sin(2 * pi * x)
    vs_world_pos = vec3(pos.x, 0.05 * sin(2 * 3.14 * (pos.x + offset)), pos.z);
    vs_world_pos.y -= 0.5;
    gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);
  }
}
