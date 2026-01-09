#version 410 core

in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUV;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D tex;

// NEW FLAGS
uniform bool u_fullbright;
uniform bool u_masked;
uniform bool u_additive;
uniform bool u_chrome;

out vec4 FragColor;

void main()
{
    vec4 texel = texture(tex, vUV);

    // --------------------------
    // MASKED: binary alpha test
    // --------------------------
    if (u_masked && texel.a < 0.5)
        discard;

    // --------------------------
    // FULLBRIGHT: no lighting
    // --------------------------
    if (u_fullbright)
    {
        FragColor = texel;
        return;
    }

    // --------------------------
    // IMPROVED LIGHTING (Ambient + Diffuse + Specular)
    // --------------------------
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPos - vWorldPos);
    vec3 V = normalize(viewPos - vWorldPos);
    vec3 H = normalize(L + V); // Half vector for specular

    // Ambient (soft base lighting)
    float ambient = 0.3;

    // Diffuse (directional light)
    float diff = max(dot(N, L), 0.0);

    // Specular (shiny highlights)
    float spec = pow(max(dot(N, H), 0.0), 32.0) * 0.2;

    // Rim light (edge lighting for better shape definition)
    float rim = pow(1.0 - max(dot(N, V), 0.0), 3.0) * 0.15;

    vec3 color = texel.rgb * (ambient + diff + spec + rim);

    // --------------------------
    // ADDITIVE BLENDING
    // (just output the color)
    // actual blend func set in C
    // --------------------------
    if (u_additive)
    {
        FragColor = vec4(color, texel.a);
        return;
    }

    // regular
    FragColor = vec4(color, texel.a);
}
