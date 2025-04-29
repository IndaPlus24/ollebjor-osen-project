$input v_texcoord0
#include <bgfx_shader.sh>

// Match the uniform names from C++ code
SAMPLER2D(u_albedo, 0);
SAMPLER2D(u_lighting, 1);

void main() {
    // Sample G-buffer
    vec4 albedoData = texture2D(u_albedo, v_texcoord0);
    vec4 lightingData = texture2D(u_lighting, v_texcoord0);
    
    // Combine albedo and lighting
    vec3 finalColor = albedoData.rgb * lightingData.rgb;
    
    // Gamma correction for better visual results
    finalColor = pow(finalColor, vec3_splat(1.0/2.2));
    
    gl_FragColor = vec4(finalColor, 1.0);
}
