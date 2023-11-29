#version 430 core
in vec2 TexCoord;

out vec4 FragColor;

const float PI = 3.1415926;
uniform sampler2D u_water;
uniform vec2 u_center;
uniform float u_radius = 0.05;
uniform float u_strength = 0.01;

void main() {
  vec4 info = texture2D(u_water, TexCoord);

  // 新增drop後，每一個texel的r的平均還要是0.5
  float drop = max(0, 1 - length(u_center - TexCoord) / u_radius);
  if (drop > 0) {
    float delta_height = (drop - 0.5) * u_strength;
    info.r += delta_height;
  }

  FragColor = info;
}
