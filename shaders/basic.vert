#version 410 core

// Input vertex attribute - position from our vertex buffer
layout(location = 0) in vec3 aPos;

// Simple rotation to see the model better
uniform float time;

void main() {
    // Simple rotation around Y axis
    float angle = time * 0.5; // Slow rotation
    float cosA = cos(angle);
    float sinA = sin(angle);
    
    // Rotate the vertex
    vec3 rotated;
    rotated.x = aPos.x * cosA - aPos.z * sinA;
    rotated.y = aPos.y;
    rotated.z = aPos.x * sinA + aPos.z * cosA;
    
    // Transform vertex position to clip space
    gl_Position = vec4(rotated.x, rotated.y, rotated.z, 1.0);
    gl_PointSize = 15.0; // Set point size in vertex shader for better compatibility
}
