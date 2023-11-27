#version 430 core
in vec2 TexCoord;

uniform sampler2D u_water;
uniform vec2 u_dx = vec2(0.01, 0);
uniform vec2 u_dy = vec2(0, 0.01);

void main() {
  vec4 info = texture2D(u_water, TexCoord);

  float average = (
        texture2D(u_water, TexCoord + u_dx).r +
        texture2D(u_water, TexCoord - u_dx).r +
        texture2D(u_water, TexCoord + u_dy).r +
        texture2D(u_water, TexCoord - u_dy).r
  ) * 0.25;

  info.g += (average - info.r) * 2.0;

  info.g *= 0.993;

  info.r += info.g;

  gl_FragColor = info;
}
