$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_baseColorFactor;
SAMPLER2D(s_texColor, 0);

void main()
{
    vec4 color = u_baseColorFactor * texture2D(s_texColor, v_texcoord0);
    gl_FragColor = color;
}