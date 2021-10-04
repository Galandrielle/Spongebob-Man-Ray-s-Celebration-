#version 330 core
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D sampler;

void main(){

	color = texture( sampler, UV );
	if (color.a > 0.01)color.a = 0.5;//semi transparent
	else discard;
	color = vec4(0.8,0.8,0.8,1);
}