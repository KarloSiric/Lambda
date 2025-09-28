#version 330 core
in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D tex;

void main() {
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(lightPos - fragPosition);
    vec3 V = normalize(viewPos - fragPosition);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), 32.0);

    vec3 ambient  = 0.20 * vec3(1.0);
    vec3 diffuse  = diff * vec3(1.0);
    vec3 specular = spec * vec3(1.0);

    vec4 texColor = texture(tex, fragTexCoord);
    vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(lighting, 1.0) * texColor;
}
