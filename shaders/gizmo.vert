#version 410 core

layout(location = 0) in vec3  aPos;
layout(location = 1) in float aAxisId;   // 0=X  1=Y  2=Z

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int  hoveredAxis;   // -1 = none
uniform int  activeAxis;    // -1 = none

out vec4 vColor;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    int axis = int(round(aAxisId));

    // Saturated axis base colours: X=red  Y=green  Z=blue
    vec3 color;
    if      (axis == 0) color = vec3(1.00, 0.20, 0.20);
    else if (axis == 1) color = vec3(0.15, 1.00, 0.15);
    else                color = vec3(0.20, 0.45, 1.00);

    float alpha = 0.90;

    if (axis == activeAxis) {
        // Bright yellow while dragging
        color = vec3(1.0, 0.90, 0.10);
        alpha = 1.0;
    } else if (axis == hoveredAxis) {
        // Brighten + fully opaque on hover
        color = clamp(color * 1.4 + vec3(0.15), 0.0, 1.0);
        alpha = 1.0;
    }

    vColor = vec4(color, alpha);
}
