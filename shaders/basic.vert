#version 410 core

// Input vertex attribute - position from our vertex buffer
layout(location = 0) in vec3 aPos;

// Camera controls from keyboard
uniform float time;
uniform float rotation_x; 
uniform float rotation_y;
uniform float zoom;

void main() {
    // Center the model (it might be offset)
    vec3 centered = aPos;
    centered.y -= 0.5; // Adjust vertical centering
    
    // Auto-rotation (slow) + manual rotation from keyboard (Y axis)
    float angleY = time * 0.2 + rotation_y;
    float cosY = cos(angleY);
    float sinY = sin(angleY);
    
    vec3 rotatedY;
    rotatedY.x = centered.x * cosY - centered.z * sinY;
    rotatedY.y = centered.y;
    rotatedY.z = centered.x * sinY + centered.z * cosY;
    
    // Apply manual rotation from keyboard (X axis) 
    float angleX = rotation_x - 0.2; // Slight default tilt
    float cosX = cos(angleX);
    float sinX = sin(angleX);
    
    vec3 rotatedXY;
    rotatedXY.x = rotatedY.x;
    rotatedXY.y = rotatedY.y * cosX - rotatedY.z * sinX;
    rotatedXY.z = rotatedY.y * sinX + rotatedY.z * cosX;
    
    // Apply zoom
    vec3 scaled = rotatedXY * zoom;
    
    // Simple perspective
    float dist = 2.0;
    vec3 final_pos = scaled;
    final_pos.z = (final_pos.z - dist) * 0.5;
    
    // Transform vertex position to clip space
    gl_Position = vec4(final_pos.x, final_pos.y, final_pos.z, 1.0);
    gl_PointSize = 10.0;
}
