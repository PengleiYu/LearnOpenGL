#version 410
layout (location=0) in vec3 position;

uniform mat4 v_matrix;
uniform mat4 proj_matrix;
uniform float tf;// 时间因子

out vec4 varyingColor;

mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(float x, float y, float z);

void main(void)
{
    float i = gl_InstanceID + tf;// 每个实例基于时间因子
    float fac_trans = 8.0;
    float a = sin(2.0f * i) * fac_trans; // 平移的x,y,z
    float b = sin(3.0f * i) * fac_trans;
    float c = sin(4.0f * i) * fac_trans;
    
    float fac_rota = 1000;
    mat4 localRotX = buildRotateX(fac_rota*i);
    mat4 localRotY = buildRotateY(fac_rota*i);
    mat4 localRotZ = buildRotateZ(fac_rota*i);
    mat4 localTrans = buildTranslate(a,b,c);
    
    mat4 newM_matrix = localTrans * localRotX * localRotY * localRotZ;
    mat4 mv_matrix = v_matrix * newM_matrix;
    gl_Position = proj_matrix * mv_matrix * vec4(position,1.0);
    
    varyingColor = vec4(position,1.0) * 0.5 + vec4(0.5,0.5,0.5,0.5);
}

mat4 buildRotateX(float rad){
    return mat4(    1.0, 0.0, 0.0, 0.0,
                0.0, cos(rad), -sin(rad), 0.0,
                0.0, sin(rad), cos(rad), 0.0,
                0.0, 0.0, 0.0, 1.0 );
}
mat4 buildRotateY(float rad){
    return mat4(    cos(rad), 0.0, sin(rad), 0.0,
                0.0, 1.0, 0.0, 0.0,
                -sin(rad), 0.0, cos(rad), 0.0,
                0.0, 0.0, 0.0, 1.0 );
}
mat4 buildRotateZ(float rad){
    return mat4(    cos(rad), sin(rad), 0.0, 0.0,
                -sin(rad), cos(rad), 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0 );
}
mat4 buildTranslate(float x, float y, float z){
    return  mat4(    1.0, 0.0, 0.0, 0.0,
                 0.0, 1.0, 0.0, 0.0,
                 0.0, 0.0, 1.0, 0.0,
                 x, y, z, 1.0 );
}
