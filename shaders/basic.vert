#version 410 core

// Input vertex attribute - position from our vertex buffer
layout(location = 0) in vec3 aPos;

// Uniform for rotation
uniform float time;

void main() {
    // Simple rotation around Y axis to see the model better
    float angle = time * 0.5;
    float cosA = cos(angle);
    float sinA = sin(angle);
    
    // Rotate around Y axis
    vec3 rotated;
    rotated.x = aPos.x * cosA - aPos.z * sinA;
    rotated.y = aPos.y;
    rotated.z = aPos.x * sinA + aPos.z * cosA;
    
    // Output with some scaling to make it more visible
    gl_Position = vec4(rotated.x * 2.0,     // Scale X
                      rotated.y * 2.0,       // Scale Y
                      0.0,                   // Fixed Z
                      1.0);
    gl_PointSize = 5.0;  // Reasonable point size
}
