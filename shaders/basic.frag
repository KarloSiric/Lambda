#version 410 core

// Output color for this fragment/pixel
out vec4 FragColor;

void main() {
    // Set a simple orange color for our triangle
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);  // Orange color (R, G, B, Alpha)
}
