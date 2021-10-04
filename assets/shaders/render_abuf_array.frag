#version 420
#define ABUFFER_SIZE 8

layout(pixel_center_integer) in vec4 gl_FragCoord;

smooth in vec4 f_pos;
smooth in vec3 f_tex_coord;
smooth in vec3 f_normal;

//flat?? no
uniform int screen_width;
uniform int screen_height;

coherent uniform layout (rgba32f) image2DArray abuf_img;
coherent uniform layout(r32ui) uimage2D counter_img;

const vec3 cool = vec3(0.1,0.1,0.9);
const vec3 warm = vec3(0.9,0.2,0.1);


//shade using green=white strips
vec3 shade_strips(vec3 texcoord){
	vec3 col;
	float i=floor(texcoord.x * 2.f);

	col.rgb=fract(i*0.5f) == 0.0f ? vec3(0.4f, 0.85f, 0.0f) : vec3(1.f);
	col.rgb*=texcoord.z;

	return col;
}

void main()
{
    ivec2 coords = ivec2(gl_FragCoord.xy);
	//Create fragment to be stored
	vec4 abuffval;
	vec3 color;
    if (coords.x >= 0 && coords.y >=0 && coords.x<screen_width && coords.y<screen_height)
    {
		//int abidx= (int)imageAtomicIncWrap(counter_img, coords, ABUFFER_SIZE);
		//this should only apply in ABUFFER_SIZE
		int abidx=imageAtomicAdd(counter_img, coords, 1);
		
		
		color=shade_strips(f_tex_coord);
		//int ab_num_frag = int(imageLoad(counter_img, coords).r); 
		//color = (float(ab_num_frag)/ABUFFER_SIZE) * warm + (1.f - float(ab_num_frag)/ABUFFER_SIZE) * cool;
		/*
		vec3 N = normalize(f_normal);
		vec3 L = normalize(vec3(0.0f,1.0f,1.0f));
		color = vec3(dot(N,L));
		*/
		abuffval.rgb=color;
		abuffval.w=f_pos.z;	//Will be used for sorting
		
		
		imageStore(abuf_img, ivec3(coords, abidx), abuffval);
	} 
	discard;
}
