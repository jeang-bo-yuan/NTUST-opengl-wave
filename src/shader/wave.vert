#version 430 core
layout(location = 0) in vec3 pos;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;
uniform uint frame;
uniform sampler2D height_map;
uniform float WAVE_SIZE;

out vec3 vs_world_pos;
out vec3 vs_normal;


void main() {
  vec2 TexCoord = clamp((vec2(pos.x , pos.z) + WAVE_SIZE) / (2.f * WAVE_SIZE), 0, 1);

  vec4 info = texture2D(height_map, TexCoord);
  vs_world_pos = vec3(pos.x, (info.r - 0.5), pos.z);
  gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);

  // x加0.001後，y的變化量
  vec4 ux = texture2D(height_map, TexCoord + vec2(0.001, 0)) - info;
  // z加0.001後，y的變化量
  vec4 uz = texture2D(height_map, TexCoord + vec2(0, 0.001)) - info;

  vs_normal = normalize(
    vec3(-ux.r, 0.001, 0) + vec3(0, 0.001, -uz.r)
  );
}
