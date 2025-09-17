#version 410 core

// Input vertex attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

// Output to fragment shader
out vec2 TexCoord;

// Camera controls from keyboard
uniform float time;
uniform float rotation_x; 
uniform float rotation_y;
uniform float zoom;

void main() {
    // Pass texture coordinate to fragment shader
    TexCoord = aTexCoord;
    
    // The model vertices
    vec3 pos = aPos;
    
    // Rotate around Y axis (vertical axis) for standard model viewing
    float angleY = time * 0.2 + rotation_y * 0.5;
    mat3 rotY = mat3(
        cos(angleY), 0.0, sin(angleY),
        0.0, 1.0, 0.0,
        -sin(angleY), 0.0, cos(angleY)
    );
    
    pos = rotY * pos;
    
    // Apply tilt (X rotation)
    float angleX = rotation_x * 0.3;
    mat3 rotX = mat3(
        1.0, 0.0, 0.0,
        0.0, cos(angleX), -sin(angleX),
        0.0, sin(angleX), cos(angleX)
    );
    
    pos = rotX * pos;
    
    // Apply zoom
    pos *= zoom * 0.5;
    
    // Move model back from camera for perspective
    pos.z -= 5.0;
    
    // Perspective projection
    float fov = 45.0 * 3.14159 / 180.0;
    float near = 0.1;
    float far = 100.0;
    float aspect = 1.0;
    
    // Simple perspective division
    float perspective = 1.0 / tan(fov * 0.5);
    
    vec4 projected;
    projected.x = pos.x * perspective / aspect;
    projected.y = pos.y * perspective;
    projected.z = (pos.z * (far + near) - 2.0 * far * near) / (pos.z * (far - near));
    projected.w = -pos.z;  // This is crucial for perspective divide
    
    gl_Position = projected;
    gl_PointSize = 5.0;
}
