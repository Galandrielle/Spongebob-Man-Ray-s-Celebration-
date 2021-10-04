#version 330 core
out vec4 FragColor;
  
in vec4 vertexColor;
in vec2 f_tex_coord;
uniform sampler2D sampler;
uniform sampler2D depth_buffer;

uniform int window_width;
uniform int window_height;
uniform int peel;

//layout(pixel_center_integer) in vec4 gl_FragCoord;
void main()
{

	vec2 pos_in_tex = gl_FragCoord.xy / vec2(window_width,window_height);
	FragColor = texture(sampler,f_tex_coord);
		if (FragColor.x < 0.01  || FragColor.a <0.1)
		discard;
	if (FragColor.a >0.01)
		FragColor.a = 1.0;
	
	//this means that there is something behind the quad (the depth is 0)



} 
