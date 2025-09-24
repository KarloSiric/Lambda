#version 410 core

in vec2 fragTexCoord;
out vec4 FragColor;
uniform float time;

void main() {
    // Orange Half-Life color with some variation
    vec3 orange = vec3(1.0, 0.6, 0.2);
    vec3 variation = vec3(0.1 * sin(fragTexCoord.x * 10.0), 0.1 * cos(fragTexCoord.y * 10.0), 0.0);
    FragColor = vec4(orange + variation, 1.0);
};
