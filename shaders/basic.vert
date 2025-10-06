// textured.vert
#version 410 core
layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aUV;

uniform mat4 model, view, projection;

out vec3 vNormal;
out vec3 vWorldPos;
out vec2 vUV;

void main( ) {
    vec4 world  = model * vec4( aPos, 1.0 );
    vWorldPos   = world.xyz;
    vNormal     = mat3( model ) * aNormal;
    vUV         = aUV;
    gl_Position = projection * view * world;
}
