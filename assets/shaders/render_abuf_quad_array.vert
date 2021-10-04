#version 420

uniform vec3 color;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
//inverse transpose view matrix
uniform mat4 view_IT;

layout (location = 0) in vec3 vertex_pos;
layout (location = 2) in vec2 vertex_tex_coord;

smooth out vec4 f_pos;
smooth out vec3 f_tex_coord;
smooth out vec3 f_color;

void main()
{
   vec4 pos = proj * view * model * vec4(vertex_pos.xyz,1.0); 

   //investigate 0-0
   vec3 normal_eye = normalize((view_IT*vec4(vec3(0.0,0.0,1.0), 1.0f)).xyz);

   f_tex_coord.xy = vertex_pos.xy;
   f_tex_coord.z = abs(normal_eye.z);

   f_pos = pos;
   f_color = color;
   gl_Position = pos;
}
