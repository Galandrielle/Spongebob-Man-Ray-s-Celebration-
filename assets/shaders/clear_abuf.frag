#version 460
// Input Variables
uniform int	layer;

//layout(binding = 0, r32ui)	
//coherent uniform uimage2DRect in_image_head;
struct NodeTypeLL
{
	float depth;
	uint color;
	uint next;
};
layout(binding = 0, r32ui)		
coherent uniform uimage2D  in_image_head;



layout(binding = 1, std430)		
coherent buffer  LinkedLists   
{ 
NodeTypeLL nodes[]; 
};

layout(binding = 2, offset = 0)
uniform atomic_uint   in_next_address;

// Output Variables
layout(location = 0, index = 0) out vec4 out_frag_color;

layout(pixel_center_integer) in vec4 gl_FragCoord;
void main(void)
{
	imageStore(in_image_head,ivec2(gl_FragCoord.xy), uvec4(0));

	atomicCounterExchange(in_next_address,0);
	

	discard;
}