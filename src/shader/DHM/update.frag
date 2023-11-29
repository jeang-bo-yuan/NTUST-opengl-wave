#version 430 core
in vec2 TexCoord;

uniform sampler2D u_water;
uniform vec2 u_dx = vec2(0.01, 0);
uniform vec2 u_dy = vec2(0, 0.01);

void main() {
  vec4 info = texture2D(u_water, TexCoord);

  // 我的height map中，0.5為平衡位置、0為最低、1為最高
  // 實際高在 -0.5 ~ 0.5

  float average_height = (
    texture2D(u_water, TexCoord + u_dx).r - 0.5 +
    texture2D(u_water, TexCoord - u_dx).r - 0.5 +
    texture2D(u_water, TexCoord + u_dy).r - 0.5 +
    texture2D(u_water, TexCoord - u_dy).r - 0.5
  ) * 0.25;

  float velocity = (info.g - 0.5) + (average_height - (info.r - 0.5)) * 2.0;

  velocity *= 0.993;

  info.r += velocity;
  info.g = velocity + 0.5;


  gl_FragColor = info;
}
