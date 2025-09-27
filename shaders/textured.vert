#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;    // REAL normals from MDL
layout (location = 2) in vec2 texCoord;  // REAL texture coordinates from MDL

// Camera uniforms
uniform float time;
uniform float rotation_x;
uniform float rotation_y;
uniform float zoom;

// Output to fragment shader
out vec2 fragTexCoord;
out vec3 worldPos;
out vec3 worldNormal;  // Pass real normals to fragment shader
out float depth;

void main() {
    // Apply zoom scaling
    vec3 scaledPos = position * zoom;
    
    // Create proper rotation matrices
    float cx = cos(rotation_x);
    float sx = sin(rotation_x);
    float cy = cos(rotation_y);
    float sy = sin(rotation_y);
    
    // Rotation matrix for Y axis (yaw)
    mat3 rotY = mat3(
        cy,  0.0, sy,
        0.0, 1.0, 0.0,
        -sy, 0.0, cy
    );
    
    // Rotation matrix for X axis (pitch)  
    mat3 rotX = mat3(
        1.0, 0.0, 0.0,
        0.0, cx,  -sx,
        0.0, sx,  cx
    );
    
    // Combined rotation matrix
    mat3 rotation = rotX * rotY;
    
    // Apply rotations to position
    vec3 rotated = rotation * scaledPos;
    
    // Apply rotations to normal as well
    vec3 rotatedNormal = rotation * normal;
    
    // Move model away from camera for proper viewing
    vec3 viewPosition = rotated + vec3(0.0, 0.0, -3.0);
    
    // Proper perspective projection matrix
    float fov = radians(45.0);
    float aspect = 800.0 / 600.0;  // Standard 4:3 aspect ratio
    float near = 0.1;
    float far = 100.0;
    
    float f = 1.0 / tan(fov * 0.5);
    
    mat4 projection = mat4(
        f/aspect, 0.0,  0.0,                            0.0,
        0.0,      f,    0.0,                            0.0,
        0.0,      0.0,  (far+near)/(near-far),         -1.0,
        0.0,      0.0,  (2.0*far*near)/(near-far),      0.0
    );
    
    // Apply projection
    vec4 projectedPos = projection * vec4(viewPosition, 1.0);
    
    gl_Position = projectedPos;
    
    // Pass data to fragment shader
    fragTexCoord = texCoord;
    worldPos = rotated;
    worldNormal = normalize(rotatedNormal);  // Use REAL normals!
    depth = -viewPosition.z;
}
