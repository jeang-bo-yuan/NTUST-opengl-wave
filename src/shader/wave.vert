#version 430 core
layout(location = 0) in vec3 pos;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;
uniform uint frame;
uniform bool draw_wave = true;

out vec3 vs_world_pos;
out vec3 vs_normal;

// y  = 0.05 * sin(2 * pi * x)
// y' = 0.05 * 2 * pi * cos(2 * pi * x)

void main() {
  if (draw_wave) {
    // changes over frames
    float offset = float(frame) / 20;

    vs_world_pos = vec3(pos.x, 0.05 * sin(2 * 3.14 * (pos.x + offset)), pos.z);
    gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);

    float slope = 0.05 * 2 * 3.14 * cos(2 * 3.14 * (pos.x + offset));
    vs_normal = normalize(vec3(-slope, 1, 0));
  }
  else {
    vs_world_pos = pos;
    gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);
    vs_normal = vec3(0, 1, 0);
  }
}
