#version 330 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 fragTexCoord;

void main() {
    vec4 worldPos = model * vec4(inPosition, 1.0);
    fragPosition  = worldPos.xyz;
    fragNormal    = mat3(transpose(inverse(model))) * inNormal;
    fragTexCoord  = inTexCoord;
    gl_Position   = projection * view * worldPos;
}
