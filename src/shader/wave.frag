in vec3 vs_world_pos;
in vec3 vs_normal;

out vec4 FragColor;

vec4 applyLight();

void main() {
  FragColor = vec4(1, 1, 1, 1);
  FragColor = applyLight();
}

uniform vec4 color_ambient = vec4(0.1, 0.2, 0.5, 1.0);
uniform vec4 color_diffuse = vec4(0.2, 0.3, 0.6, 1.0);
uniform vec4 color_specular = vec4(1.0, 1.0, 1.0, 1.0);
uniform float shininess = 77.0f;
uniform vec3 light_position = vec3(0, 2, 0);
uniform vec3 eye_position;

vec4 applyLight() {
    vec3 light_direction = normalize(light_position - vs_world_pos);
    vec3 EyeDirection = normalize(eye_position - vs_world_pos);
    vec3 half_vector = normalize(light_direction + EyeDirection);

    float diffuse = max(0.0, dot(vs_normal, light_direction));
    float specular = pow(max(0.0, dot(vs_normal, half_vector)), shininess);

    return min(FragColor * color_ambient, vec4(1.0)) + diffuse * color_diffuse + specular * color_specular;
}
