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
    // NORMAL LIGHTING
    // --------------------------
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPos - vWorldPos);
    float diff = max(dot(N, L), 0.0);

    vec3 color = texel.rgb * (0.2 + 0.8 * diff);

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
