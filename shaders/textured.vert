#version 410 core

// Input vertex attribute - position from our vertex buffer
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

// Output to fragment shader
out vec2 TexCoord;

// Uniforms for camera control
uniform float rotation_x;
uniform float rotation_y;
uniform float zoom;

void main() {
    // Pass texture coordinates to fragment shader
    TexCoord = aTexCoord;
    
    // Apply transformations
    vec3 pos = aPos;
    
    // Rotation around Y axis (horizontal)
    float cosY = cos(rotation_y);
    float sinY = sin(rotation_y);
    vec3 rotatedY;
    rotatedY.x = pos.x * cosY - pos.z * sinY;
    rotatedY.y = pos.y;
    rotatedY.z = pos.x * sinY + pos.z * cosY;
    
    // Rotation around X axis (vertical tilt)
    float cosX = cos(rotation_x);
    float sinX = sin(rotation_x);
    vec3 rotatedXY;
    rotatedXY.x = rotatedY.x;
    rotatedXY.y = rotatedY.y * cosX - rotatedY.z * sinX;
    rotatedXY.z = rotatedY.y * sinX + rotatedY.z * cosX;
    
    // Apply zoom
    vec3 scaled = rotatedXY * zoom;
    
    // Final position
    gl_Position = vec4(scaled, 1.0);
    gl_PointSize = 10.0;
}
