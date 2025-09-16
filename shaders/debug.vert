#version 410 core

// Input vertex attribute - position from our vertex buffer
layout(location = 0) in vec3 aPos;

// Uniform for rotation
uniform float time;

void main() {
    // Simple pass-through with slight rotation for visibility
    float angle = time * 0.3;
    float cosA = cos(angle);
    float sinA = sin(angle);
    
    // Rotate around Y axis only
    vec3 rotated;
    rotated.x = aPos.x * cosA - aPos.z * sinA;
    rotated.y = aPos.y;
    rotated.z = aPos.x * sinA + aPos.z * cosA;
    
    // Simple orthographic projection
    gl_Position = vec4(rotated.x, rotated.y, rotated.z * 0.01, 1.0);
    gl_PointSize = 2.0;  // Small points to see structure better
}
