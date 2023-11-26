#version 430 core
in vec2 texCoord;

uniform sampler2D color_buffer;

out vec4 FragColor;

void main () {
  FragColor = texture(color_buffer, floor(texCoord * 100.f) / 100.f);
}
