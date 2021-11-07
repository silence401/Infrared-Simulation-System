#version 330 core

in vec3 a_position;
in vec4 a_color;
in vec3 a_normals;
in vec2 a_texcoord;
out vec2 TexCoords;
out vec4 v_color;

uniform mat4 u_pmv_matrix;
//uniform float u_point_size;

void main(void)
{
    //TexCoords = a_texcoord;    
    gl_Position = u_pmv_matrix * vec4(a_position, 1);
   // gl_Position = vec4(a_position, 1);
    gl_PointSize = 1.0f;
    v_color = a_color;
}
