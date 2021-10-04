#version 420
#define ABUFFER_SIZE 8
uniform int screen_width;
uniform int screen_height;

layout(pixel_center_integer) in vec4 gl_FragCoord;

smooth in vec4 f_pos;

out vec4 out_frag_color;

uniform layout(r32ui) uimage2D counter_img;
uniform layout(rgba32f) image2DArray abuf_img;

const vec3 cool = vec3(0.1,0.1,0.9);
const vec3 warm = vec3(0.9,0.2,0.1);

const float fragment_alpha=0.5f;
//const vec4 background_color = vec4(0.5,0.5,0.9,1);
uniform vec4 background_color;
//local memory array
vec4 fragment_list[ABUFFER_SIZE];

void bubble_sort(int array_size) {
  for (int i = (array_size - 2); i >= 0; --i) {
    for (int j = 0; j <= i; ++j) {
      if (fragment_list[j].w > fragment_list[j+1].w) {
		vec4 temp = fragment_list[j+1];
		fragment_list[j+1] = fragment_list[j];
		fragment_list[j] = temp;
      }
    }
  }
}
//fill the array with fragments from the 3D image (2D array)
void fill_frag_array(ivec2 coords, int ab_num_frag){
	//Load fragments into a local memory array for sorting
	for(int i=0; i<ab_num_frag; i++){
		fragment_list[i]=imageLoad(abuf_img, ivec3(coords.x,coords.y, i));

	}
}
//keep only the closest fragments
vec4 resolve_closest(ivec2 coords, int ab_num_frag){

	//Search smallest z
	vec4 minFrag=vec4(0.0f, 0.0f, 0.0f, 1000000.0f);
	for(int i=0; i<ab_num_frag; i++){
		vec4 val=imageLoad(abuf_img, ivec3(coords, i));
		if(val.w<minFrag.w){
			minFrag=val;
		}
	}
	//Output final color for the frame buffer
	return minFrag;
}

vec4 resolve_alpha_blend(ivec2 coords, int ab_num_frag){
	
	//Copy fragments in local array from image
	fill_frag_array(coords, ab_num_frag);

	//Sort fragments in local memory array
	bubble_sort(ab_num_frag);
		
	vec4 final_color=vec4(0.0f);

	final_color=vec4(0.0f);
	for(int i=0; i<ab_num_frag; i++){
		vec4 frag=fragment_list[i];
		
		vec4 col;
		col.rgb=frag.rgb;
		col.rgb = (float(ab_num_frag)/ABUFFER_SIZE) * warm + (1.f - float(ab_num_frag)/ABUFFER_SIZE) * cool;
		col.w=fragment_alpha;	//uses constant alpha

		col.rgb=col.rgb*col.w;

		final_color=final_color+col*(1.0f-final_color.a);
	}

	//final_color=final_color+background_color*(1.0f-final_color.a);

	return final_color;

}

void main(void)
{
    ivec2 coords = ivec2(gl_FragCoord.xy);
    if (coords.x >= 0 && coords.y >= 0 && coords.x < screen_width && coords.y < screen_height)
    {
        int ab_num_frag = int(imageLoad(counter_img, coords).r);

        if (ab_num_frag < 0)
            ab_num_frag = 0;
        if (ab_num_frag > ABUFFER_SIZE)
            ab_num_frag = ABUFFER_SIZE;

        if (ab_num_frag > 0){
		    //out_frag_color = resolve_closest(coords,ab_num_frag);
			out_frag_color = resolve_alpha_blend(coords, ab_num_frag);
		}else{discard;}
    }

}

