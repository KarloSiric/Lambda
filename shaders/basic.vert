#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
uniform float time;
uniform float rotation_x;
uniform float rotation_y;
uniform float zoom;
out vec2 fragTexCoord;

void main() {
    // Apply transformations\n
    vec3 pos = position * zoom;
    // Rotation matrices\n
    float cx = cos(rotation_x);
    float sx = sin(rotation_x);
    float cy = cos(rotation_y);
    float sy = sin(rotation_y);
    // Apply rotations\n
    vec3 rotated;
    rotated.x = pos.x * cy - pos.z * sy;
    rotated.y = pos.x * sx * sy + pos.y * cx + pos.z * sx * cy;
    rotated.z = pos.x * cx * sy - pos.y * sx + pos.z * cx * cy;
    gl_Position = vec4(rotated, 1.0);
    fragTexCoord = texCoord;
};
