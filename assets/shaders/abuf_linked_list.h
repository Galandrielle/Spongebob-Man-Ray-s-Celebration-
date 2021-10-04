#version 420

#ifndef ABUF_LINKED_LIST_H
#define ABUF_LINKED_LIST_H

uniform int screen_width;
uniform int screen_height;

#define ABUFFER_SIZE 16

niform int abuf_shared_pool_size;

coherent uniform layout(float) uimage2D abuf_page_idx_img;
coherent uniform layout(float) uimage2D abuf_frag_count_img;
coherent uniform layout(float) uimage2D semaphore_img;

coherent uniform layout(size4x32) imageBuffer shared_page_list;
coherent uniform layout(float) uimageBuffer shared_link_list_img;

//next available page in the shaderpool
coherent uniform uint d_cur_shared_page;

//put 1 in semaphore, if it returns(was) 0, then
//the operation was succesful (TSL)
bool semaphore_acquire(ivec2 coords)
{
    return imageAtomicExchange(semaphore_img, coords, 1U) == 0U;
}

//set semaphore at coords to 0 (release)
void semaphore_release(ivec2 coords)
{
    return imageAtomicExchange(semaphore_img,coords, 0U);
}

bool get_semaphore(ivec2 coords)
{
    return imageLoad(semaphore_img, coords).x == 0;
}

//if val >0 then put 1, else 0
void set_semaphore(ivec2 coords, bool val)
{
    imageStore(semaphore_img, coords, uvec4(val ? 1U : 0U, 0U, 0U, 0U));
}

uint get_pixel_current_page(ivec2 coords)
{
    return imageLoad(abuf_page_idx_img, coords).x;
}

void set_pixel_current_page(ivec2 coords, uint new_page_idx)
{
    imageStore(abuf_page_idx_img, coords, uvec4(new_page_idx,0U,0U,0U));
}

void get_pixel_frag_counter(ivec2 coords)
{
    return imageLoad(abuf_frag_count_img, coords).x;
}

void set_pixel_frag_counter(ivec2 coords, uint val)
{
    imageStore(abuf_frag_count_img, coords, uvec4(val,0U,0U,0U));
}

uint pixel_frag_counter_atomic_add(ivec2 coords, uint val)
{
    return imageAtomicAdd(abuf_frag_count_img, coords, val);
}


//find where in the global pool the first
//node of our list lies
uint shared_pool_get_link(unit page_num)
{
    return imageLoad(shared_link_list_img, (int)(page_num)).x;
}

//set a new location for the first node
//of the linked list of this pixel
void shared_pool_set_link(uint page_num, uint pointer)
{
    imageStore(shared_link_list_img, (int)(page_num), uvec4(pointer,0U,0U,0U));
}

//get the whole page (4 elements) for a given index
vec4 shared_pool_get_value(uint index)
{
    return imageLoad(shared_page_list_img, (int)(index));
}

//set the entire page for a given index
void shared_pool_set_value(uint index, vec4 val)
{
    imageStore(shared_page_list_img, (int)(index), val);
}

#endif
