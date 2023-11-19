#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform uint frame;

out vec3 tex_coord;

void main() {
  gl_Position = proj_matrix * view_matrix * vec4(position, 1);
  tex_coord = position;
}
