#version 410 core

in vec2 fragTexCoord;
in vec3 worldPos;
in vec3 worldNormal;  // Real normals from MDL file
in float depth;

out vec4 FragColor;

uniform float time;

void main() {
    // Base Half-Life orange color scheme
    vec3 baseColor = vec3(0.95, 0.55, 0.15);  // Orange
    vec3 darkColor = vec3(0.4, 0.2, 0.05);    // Dark orange/brown for shadows
    vec3 lightColor = vec3(1.0, 0.8, 0.4);    // Light orange/yellow for highlights
    
    // Use REAL normals from the MDL file (not fake ones)
    vec3 normal = normalize(worldNormal);
    
    // Multiple light sources for better 3D appearance
    vec3 keyLight = normalize(vec3(1.0, 1.0, 1.5));    // Main light (top-right-front)
    vec3 fillLight = normalize(vec3(-0.5, 0.5, 1.0));  // Fill light (top-left-front)
    vec3 rimLight = normalize(vec3(0.0, -1.0, -1.0));  // Rim light (bottom-back)
    
    // Calculate lighting for each light source
    float keyLighting = max(dot(normal, keyLight), 0.0) * 0.8;
    float fillLighting = max(dot(normal, fillLight), 0.0) * 0.3;
    float rimLighting = max(dot(normal, rimLight), 0.0) * 0.4;
    
    // Ambient lighting so nothing is completely black
    float ambient = 0.3;
    
    // Combine all lighting
    float totalLight = ambient + keyLighting + fillLighting + rimLighting;
    totalLight = clamp(totalLight, 0.0, 1.0);
    
    // Add depth-based fog for better 3D perception
    float fogFactor = 1.0 - clamp(depth / 10.0, 0.0, 0.7);
    totalLight *= fogFactor;
    
    // Create the final color based on lighting intensity
    vec3 finalColor;
    if (totalLight < 0.4) {
        // Dark areas - use dark color
        finalColor = mix(darkColor, baseColor, totalLight * 2.5);
    } else {
        // Bright areas - mix base color with light color
        finalColor = mix(baseColor, lightColor, (totalLight - 0.4) * 1.67);
    }
    
    // Add some edge highlighting for better shape definition
    vec3 viewDir = normalize(-worldPos);
    float fresnel = 1.0 - max(dot(viewDir, normal), 0.0);
    fresnel = pow(fresnel, 3.0) * 0.2;
    finalColor += vec3(0.3, 0.15, 0.05) * fresnel;
    
    // Add subtle texture variation based on UV coordinates
    float texVariation = sin(fragTexCoord.x * 30.0) * cos(fragTexCoord.y * 30.0) * 0.03;
    finalColor += vec3(texVariation);
    
    // Very subtle animation pulse
    float pulse = sin(time * 0.3) * 0.01 + 1.0;
    finalColor *= pulse;
    
    // Ensure color stays in valid range
    finalColor = clamp(finalColor, 0.0, 1.0);
    
    FragColor = vec4(finalColor, 1.0);
}
