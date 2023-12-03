#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;
uniform float WAVE_SIZE = 5;

out vec3 vs_world_pos;
out vec3 vs_normal;

void main() {
  // 向下移
  vs_world_pos = aPos - vec3(0, WAVE_SIZE, 0); // 世界座標
  vs_world_pos.y += 0.5;
  gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);

  // 我用-aNormal當作法向量，因為Box_VAO提供的normal是指向水槽外
  // 但我希望法向量是指向水槽內
  vs_normal = -aNormal; // 世界座標下的法向量
}
