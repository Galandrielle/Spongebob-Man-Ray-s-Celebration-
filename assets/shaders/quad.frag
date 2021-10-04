#version 330 core
out vec4 FragColor;
  
in vec4 vertexColor;
in vec2 f_tex_coord;
uniform sampler2D sampler;
uniform sampler2D depth_buffer;

void main()
{
	FragColor = texture(sampler,f_tex_coord);
	FragColor.a = 0.5;
	
	//this means that there is something behind the quad
} 
