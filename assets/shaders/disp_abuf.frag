#version 460
//#include "define.h"
//#include "data_structs.h"
//#include "sort.h"
struct NodeTypeLL
{
	float depth;
	uint color;
	uint next;
};
#define LOCAL_SIZE 32

vec2 fragments [LOCAL_SIZE];
const vec3 cool = vec3(0.2,0.1,0.5);
const vec3 warm = vec3(0.9,0.2,0.1);


// Input Variables
uniform int	layer;
float fragment_alpha = 0.5;

//layout(binding = 0, r32ui)	
//coherent uniform uimage2DRect in_image_head;

layout(binding = 0, r32ui)		
coherent uniform uimage2D  in_image_head;

layout(binding = 1, std430)		
coherent buffer  LinkedLists   
{ 
NodeTypeLL nodes[]; 
};

// Output Variables
layout(location = 0, index = 0) out vec4 out_frag_color;


void sort_insert(const int num)
{
	for (int j = 1; j < num; ++j)
	{
		vec2 key = fragments[j];
		int i = j - 1;

		while (i >= 0 && fragments[i].g > key.g)
		{
			fragments[i+1] = fragments[i];
			--i;
		}
		fragments[i+1] = key;
	}
}

void sort(const int num)
{
	sort_insert(num);
}
vec4 resolve_alpha_blend(ivec2 coords, int ab_num_frag){
		
	vec4 final_color=vec4(0.0f);

	final_color=vec4(0.0f);
	for(int i=0; i<ab_num_frag; i++){
		vec4 frag= unpackUnorm4x8(floatBitsToUint(fragments[i].r));
		
		vec4 col;
		col.rgb=frag.rgb;
		col.rgb = (float(ab_num_frag)/8) * warm + (1.f - float(ab_num_frag)/8) * cool;
		col.w=fragment_alpha;	//uses constant alpha

		col.rgb=col.rgb*col.w;

		final_color=final_color+col*(1.0f-final_color.a);
	}

	//final_color=final_color+background_color*(1.0f-final_color.a);

	return final_color;

}




layout(pixel_center_integer) in vec4 gl_FragCoord;
void main(void)
{
	// Get head pixel pointer
	uint index = imageLoad(in_image_head, ivec2(gl_FragCoord.xy)).x;
	if(index == 0u)
		discard;

	// Alloc optionally, used for fixing next fragment pointers
	uint fragments_id[LOCAL_SIZE];

	// Store fragment data values to a local array
	int count = 0;
	while (index != 0u)
	{
		fragments_id[count] = index; // Set optionally
		fragments[count++]  = vec2(uintBitsToFloat(nodes[index].color), nodes[index].depth);
		index			    = nodes[index].next;
	}
	
	// Sort fragments by their depth
    sort(count);

/*
	// Use optionally if you want to perform subsequent operations in a following pass
	{
		// Set head pixel pointer
		imageStore(in_image_head, ivec2(gl_FragCoord.xy), uvec4(fragments[0].g, 0.0u, 0.0u, 0.0u));
			
		// Correct fragment connection pointers
		for(int i=0; i<count-1; i++)
			nodes[fragments_id[i]].next = fragments_id[i+1];
		nodes[fragments_id[counter-1]].next = 0U;
	}
*/

	if (count == 0)discard;
	// Return the color value of the selected fragment
   	//out_frag_color = unpackUnorm4x8(floatBitsToUint(fragments[layer].r));
	out_frag_color = resolve_alpha_blend(ivec2(gl_FragCoord.xy), count);
	//if (out_frag_color.x < 0.1)discard;
	//out_frag_color.a = 1.0;
}