#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

// vertex shader's output
in vec3 vs_world_pos[3];

// pass these to fragment shader
out vec3 geom_world_pos;
out vec3 geom_normal;

void main() {
  vec3 v1 = vs_world_pos[1] - vs_world_pos[0];
  vec3 v2 = vs_world_pos[2] - vs_world_pos[0];
  vec3 normal = normalize(cross(v1, v2));

  for (int i = 0; i < 3; ++i) {
    gl_Position = gl_in[i].gl_Position;
    geom_world_pos = vs_world_pos[i];
    geom_normal = normal;
    EmitVertex();
  }

  EndPrimitive();
}
