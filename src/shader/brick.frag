#version 430 core
in vec3 vs_world_pos;
in vec3 vs_normal;
layout (std140, binding = 1) uniform LightBlock {
  vec4 eye_position;
  vec4 light_position;
} Light;


uniform vec4 color_ambient = vec4(0.1, 0.2, 0.5, 1.0);
uniform vec4 color_diffuse = vec4(0.2, 0.3, 0.6, 1.0);
uniform vec4 color_specular = vec4(0, 0, 0, 1.0);
uniform float shininess = 1.0f;

void main() {
  gl_FragColor = vec4(1, 1, 1, 1);

  vec3 light_direction;
  if (Light.light_position.w == 0) {
    light_direction = normalize(Light.light_position.xyz);
  }
  else {
    light_direction = normalize(Light.light_position.xyz - vs_world_pos);
  }
  vec3 EyeDirection = normalize(Light.eye_position.xyz - vs_world_pos);
  vec3 half_vector = normalize(light_direction + EyeDirection);

  float diffuse = max(0.0, dot(vs_normal, light_direction));
  float specular = pow(max(0.0, dot(vs_normal, half_vector)), shininess);
  gl_FragColor = min(gl_FragColor * color_ambient, vec4(1.0)) + diffuse * color_diffuse + specular * color_specular;

  if (dot(EyeDirection, vs_normal) < 0) gl_FragColor.a = 0.5f;
}
