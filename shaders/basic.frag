#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform bool useTexture;

void main() {
    if (useTexture) {
        vec4 texColor = texture(ourTexture, TexCoord);
        
        // Handle transparency in Half-Life textures
        if (texColor.a < 0.1)
            discard;
            
        FragColor = texColor;
    } else {
        // Fallback: scientist lab coat colors
        vec2 coord = gl_FragCoord.xy / vec2(1200.0, 800.0);
        vec3 labCoat = mix(vec3(0.9, 0.95, 1.0), vec3(0.7, 0.8, 0.9), coord.y);
        float variation = sin(coord.x * 10.0) * 0.1;
        labCoat += variation;
        FragColor = vec4(labCoat, 1.0);
    }
}
