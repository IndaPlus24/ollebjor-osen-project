$input v_texcoord0, v_normal

/*
 * Copyright 2011-2025 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "bgfx_shader.sh"
#include "shaderlib.sh"

SAMPLER2D(s_texColor, 0);

void main()
{
    vec4 color = toLinear(texture2D(s_texColor, v_texcoord0));
    gl_FragColor.rgb = color.rgb;
    gl_FragColor.a = 1.0;
    gl_FragColor = toGamma(gl_FragColor);
}
