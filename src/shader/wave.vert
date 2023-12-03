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
out vec3 vs_normal;


void main() {
  if (use_height_map) {
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
  else {
    // changes over frames
    float offset = float(frame) / 20;

    // y  = 0.05 * sin(2 * pi * x)
    // y' = 0.05 * 2 * pi * cos(2 * pi * x)
    vs_world_pos = vec3(pos.x, 0.05 * sin(2 * 3.14 * (pos.x + offset)), pos.z);
    vs_world_pos.y -= 0.5;
    gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);

    float slope = 0.05 * 2 * 3.14 * cos(2 * 3.14 * (pos.x + offset));
    vs_normal = normalize(vec3(-slope, 1, 0));
  }
}
