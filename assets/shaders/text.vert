#version 330 core
layout(location = 0) in vec2 vertexPosition_screenspace;
layout(location = 1) in vec2 vertexUV;

uniform int window_width;
uniform int window_height;

out vec2 UV;
void main(){
	vec2 vertpos_homogen = vertexPosition_screenspace - vec2(window_width,window_height); // [0..w][0..h] -> [-w..w][-h..h]
	vertpos_homogen /= vec2(window_width,window_height);
	gl_Position =  vec4(vertpos_homogen,0,1);
	
	UV = vertexUV;
}

