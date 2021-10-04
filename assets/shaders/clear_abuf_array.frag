#version 420

uniform int screen_width;
uniform int screen_height;


coherent uniform layout(r32ui) uimage2D counter_img;
coherent uniform layout(rgba32f) image2DArray abuf_img;

//built-in
//in vec4 gl_FragCoord;
layout(pixel_center_integer) in vec4 gl_FragCoord;

void main(void)
{
    ivec2 coords = ivec2(gl_FragCoord.xy);

    if (coords.x >= 0 && coords.y >=0 && coords.x<screen_width && coords.y<screen_height)
    {
        //set counter to 0
        imageStore(counter_img, coords, ivec4(0));
        
        //put black in first layer
        //imageStore(abuf_img, ivec3(coords, 0), vec4(0.0f));
		imageStore(abuf_img, ivec3(coords, 0), vec4(0));
    }

    //discard fragment so that it isn't written in framebuffer
    discard;
}
