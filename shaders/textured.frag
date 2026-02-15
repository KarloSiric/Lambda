#version 410 core

in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUV;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;        // Light color RGB (0-1)
uniform float ambientIntensity; // Ambient intensity (0-1)
uniform sampler2D tex;

// Texture/render flags
uniform bool u_fullbright;
uniform bool u_masked;
uniform bool u_additive;
uniform bool u_chrome;
uniform bool u_lightingEnabled; // Global lighting toggle
uniform bool u_wireframeOverlay; // Render as solid wireframe color
uniform vec3 u_wireframeColor;   // Wireframe overlay color
uniform bool u_flatShading;       // Flat shading mode (per-face normals)

out vec4 FragColor;

void main()
{
    // --------------------------
    // WIREFRAME OVERLAY: solid color output
    // --------------------------
    if (u_wireframeOverlay)
    {
        FragColor = vec4(u_wireframeColor, 1.0);
        return;
    }

    vec4 texel = texture(tex, vUV);

    // --------------------------
    // MASKED: binary alpha test
    // --------------------------
    if (u_masked && texel.a < 0.5)
        discard;

    // --------------------------
    // FULLBRIGHT or LIGHTING DISABLED: no lighting
    // --------------------------
    if (u_fullbright || !u_lightingEnabled)
    {
        FragColor = texel;
        return;
    }

    // --------------------------
    // IMPROVED LIGHTING (Ambient + Diffuse + Specular)
    // --------------------------
    vec3 N;
    if (u_flatShading) {
        // Compute flat face normal using screen-space derivatives
        vec3 dPdx = dFdx(vWorldPos);
        vec3 dPdy = dFdy(vWorldPos);
        N = normalize(cross(dPdx, dPdy));
    } else {
        N = normalize(vNormal);
    }
    vec3 L = normalize(lightPos - vWorldPos);
    vec3 V = normalize(viewPos - vWorldPos);
    vec3 H = normalize(L + V); // Half vector for specular

    // Ambient (controlled by ambientIntensity)
    float ambient = ambientIntensity;

    // Diffuse (directional light)
    float diff = max(dot(N, L), 0.0);

    // Specular (shiny highlights)
    float spec = pow(max(dot(N, H), 0.0), 32.0) * 0.2;

    // Rim light (edge lighting for better shape definition)
    float rim = pow(1.0 - max(dot(N, V), 0.0), 3.0) * 0.15;

    // Apply light color to diffuse and specular
    vec3 litColor = texel.rgb * ambient +
                    texel.rgb * lightColor * diff +
                    lightColor * spec +
                    texel.rgb * rim;

    // --------------------------
    // ADDITIVE BLENDING
    // (just output the color)
    // actual blend func set in C
    // --------------------------
    if (u_additive)
    {
        FragColor = vec4(litColor, texel.a);
        return;
    }

    // regular
    FragColor = vec4(litColor, texel.a);
}
