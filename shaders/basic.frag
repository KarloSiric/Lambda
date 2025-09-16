#version 410 core

// Input from vertex shader
in vec2 TexCoord;

// Output color for this fragment/pixel
out vec4 FragColor;

// Texture sampler
uniform sampler2D texture1;
uniform bool useTexture;

void main() {
    if (useTexture) {
        // Sample the texture
        FragColor = texture(texture1, TexCoord);
    } else {
        // Fallback orange color
        FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    }
}
