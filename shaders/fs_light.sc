$input v_texcoord0
#include <bgfx_shader.sh>

// Match the uniform names from C++ code
SAMPLER2D(u_normal, 0);
SAMPLER2D(u_depth, 1);

void main() {
    // Sample G-buffer
    vec4 normalData = texture2D(u_normal, v_texcoord0);
    
    // Unpack normal from [0,1] to [-1,1] range
    vec3 normal = normalize(normalData.xyz * 2.0 - 1.0);
    
    // Simple directional light
    vec3 lightDir = normalize(vec3(1.0, 1.0, -0.5));
    vec3 lightColor = vec3(1.0, 0.9, 0.8);
    
    // Calculate lighting
    float diffuseStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;
    vec3 ambient = vec3(0.2, 0.2, 0.25);
    
    // Combine lighting with albedo
    vec3 finalColor = (ambient + diffuse);
    
    gl_FragColor = vec4(finalColor, 1.0);
}
