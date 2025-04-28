$input v_texcoord0, v_normal, v_tangent

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);
SAMPLER2D(s_texNormal, 1);

void main() {
    vec4 color = texture2D(s_texColor, v_texcoord0);

    // Sample and unpack normal map
    vec3 normalMap = texture2D(s_texNormal, v_texcoord0).xyz;
    normalMap = normalMap * 2.0 - 1.0; // unpack from [0,1] to [-1,1]

    // Build TBN matrix
    vec3 N = normalize(v_normal);
    vec3 T = normalize(v_tangent);
    vec3 B = normalize(cross(N, T));

    mat3 TBN = mat3(T, -B, N);

    // Transform normal map from tangent space to world space
    vec3 normal = normalize(TBN * normalMap);

    // Pack normal into [0,1] range
    vec3 packedNormal = normal * 0.5 + 0.5;

    // Output to G-buffer
    gl_FragData[0] = vec4(color.rgb, 1.0);     // Albedo
    gl_FragData[1] = vec4(packedNormal, 1.0);  // Normal
}
