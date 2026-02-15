#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float u_pointSize;

void main() {
    vColor = aColor;

    // Apply axis remap (Half-Life → OpenGL coordinates)
    // Half-Life: X=right, Y=forward, Z=up
    // OpenGL: X=right, Y=up, Z=back
    // Transform: (x, y, z) → (x, z, -y)
    vec3 remappedPos;
    remappedPos.x = aPos.x;
    remappedPos.y = aPos.z;    // Z → Y (up)
    remappedPos.z = -aPos.y;   // -Y → Z (back)

    gl_Position = projection * view * model * vec4(remappedPos, 1.0);
    gl_PointSize = u_pointSize;
}
