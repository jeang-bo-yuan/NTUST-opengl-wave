#version 430 core
layout(location = 0) in vec3 pos;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform uint frame;

out vec3 vs_world_pos;
out vec3 vs_normal;

// y  = 0.1 * sin(2 * pi * x)
// y' = 0.1 * 2 * pi * cos(2 * pi * x)

void main() {
  // changes over frames
  float offset = float(frame) / 20;

  vs_world_pos = vec3(pos.x, 0.1 * sin(2 * 3.14 * (pos.x + offset)), pos.z);
  gl_Position = proj_matrix * view_matrix * vec4(vs_world_pos, 1);

  float slope = 0.1 * 2 * 3.14 * cos(2 * 3.14 * (pos.x + offset));
  vs_normal = normalize(vec3(-slope, 1, 0));
}
