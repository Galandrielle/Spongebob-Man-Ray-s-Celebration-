#version 430

struct NodeTypeLL
{
	float depth;
	uint color;
	uint next;
};
//layout(binding = 0, r32ui)		
//coherent uniform uimage2DRect  in_image_head;

layout(binding = 0, r32ui)		
coherent uniform uimage2D  in_image_head;


layout(binding = 1, std430)		
coherent buffer  LinkedLists   
{ 
NodeTypeLL nodes[]; 
};

layout(binding = 2, offset = 0)
uniform atomic_uint   in_next_address;



smooth in vec4 f_pos;
smooth in vec3 f_tex_coord;
smooth in vec3 f_normal;
vec4 computePixelColor()
{
	return vec4(1.0,0.5,0.3,0.3);
}
//layout(pixel_center_integer) in vec4 gl_FragCoord;
void main(void)
{
    ivec2 coords = ivec2(gl_FragCoord.xy);

	// Get the next available location in the global buffer
	uint index = atomicCounterIncrement(in_next_address) + 1U;
	
	// Check for memory overflow
	if(index < nodes.length())
	{
		// Compute the shading color of each incoming fragment
		vec4 color = computePixelColor();
		
		// Store fragment data into the global buffer
		nodes[index].color = packUnorm4x8(color);
		nodes[index].depth = gl_FragCoord.z;
		// Connect fragment with head of the fragment list and then set it as the new head
		nodes[index].next  = imageAtomicExchange(in_image_head, ivec2(gl_FragCoord.xy), index);
		// Used for memory counting of overflowed fragments
		
	}
	discard;
}
