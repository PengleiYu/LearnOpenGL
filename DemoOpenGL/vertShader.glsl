#version 410
layout (location=0) in vec3 pos;
layout (location=1) in vec2 texCoord;
out vec2 tc;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform sampler2D samp;// 不能使用binding layout

void main(void)
{    gl_Position = proj_matrix * mv_matrix * vec4(pos,1.0);
    tc = texCoord;
} 
