// textured.frag
#version 410 core
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUV;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D tex;

out vec4 FragColor;

void main() {
  vec3 N = normalize(vNormal);
  vec3 L = normalize(lightPos - vWorldPos);
  float diff = max(dot(N, L), 0.0);
  vec3 base = texture(tex, vUV).rgb;
  // simple lambert + a little ambient
  vec3 color = base * (0.2 + 0.8 * diff);
  FragColor = vec4(color, 1.0);
}
