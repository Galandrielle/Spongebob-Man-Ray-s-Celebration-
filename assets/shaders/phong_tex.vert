#version 330 core

layout (location = 0) in vec3 vertex_pos; 
layout (location = 1) in vec3 n;
layout (location = 2) in vec2 tex_coord;
  
out vec3 f_normal;
out vec3 w_frag_pos;
out vec2 f_texcoords;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = vec4(vertex_pos,1.0);
	gl_Position = proj * view * model * gl_Position;
	f_normal = mat3(transpose(inverse(model))) * n; //normal scaling
	w_frag_pos = vec3(model * vec4(vertex_pos,1.0));
	f_texcoords = tex_coord;
}
