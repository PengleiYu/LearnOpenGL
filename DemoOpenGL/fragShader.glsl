#version 410
in vec2 tc;
out vec4 color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform sampler2D samp;// 不能使用binding layout

void main(void)
{    color = texture(samp, tc);
}
