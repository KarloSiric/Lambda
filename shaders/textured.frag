#version 410 core

// Input from vertex shader
in vec2 TexCoord;

// Output color
out vec4 FragColor;

// Texture sampler
uniform sampler2D texture1;
uniform int useTexture;

void main() {
    if (useTexture == 1) {
        // Sample the texture
        vec4 texColor = texture(texture1, TexCoord);
        
        // Handle transparency (common in Half-Life models)
        if (texColor.a < 0.5) {
            discard;  // Discard transparent pixels
        }
        
        FragColor = texColor;
    } else {
        // Fallback to orange color if no texture
        FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    }
}
