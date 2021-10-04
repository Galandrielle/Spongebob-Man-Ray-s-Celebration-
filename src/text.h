#ifndef TEXT_H
#define TEXT_H

#include "shader.h"
#include "texture.h"

//NOTE(ilias): this is extremely slow,
//only use for debugging text (until SDF font implementation)
typedef struct BitmapFont 
{
    GLuint VAO, vertex_buffer, uv_buffer;
    Texture tex;
    Shader s;
}BitmapFont;

static void 
init_text(BitmapFont* f, const char * tex_path)
{
    glGenVertexArrays(1, &f->VAO);
    glGenBuffers(1, &f->vertex_buffer);
    glGenBuffers(1, &f->uv_buffer);

    shader_load(&f->s, "../assets/shaders/text.vert", "../assets/shaders/text.frag");
    load_texture(&f->tex,tex_path);
}

static void 
print_text(BitmapFont *f,const char*text, i32 x,i32 y, i32 size)
{

    u32 length = strlen(text);

    vec2 *vertices = (vec2*)arena_alloc(&global_platform.frame_storage, sizeof(vec2) * 6 * length);
    u32 vertices_index = 0;

    vec2 *uvs = (vec2*)arena_alloc(&global_platform.frame_storage, sizeof(vec2) * 6 * length);
    u32 uvs_index = 0;
    for (u32 i = 0; i < length; ++i)
    {
        /*
        For every character we need to render we have to find its screen coordinate
        positions, meaning its corners (since we are rendering a quad, and then pass it
        to our vertex buffer as 6 points, meaning 2 triangles so we can render
        (this is basically where each character goes in screen(?) space)
        */
        vec2 v_up_left = {(f32)x + i*size, (f32)y+size};
        vec2 v_up_right = {(f32)x + i*size + size, (f32)y+size};
        vec2 v_down_left = {(f32)x + i*size, (f32)y};
        vec2 v_down_right = {(f32)x + i*size + size, (f32)y};

        //vertices.push_back(v_up_left);
        //vertices.push_back(v_down_left);
        //vertices.push_back(v_up_right); //should be up right???
        vertices[vertices_index++] = v_up_left;
        vertices[vertices_index++] = v_down_left;
        vertices[vertices_index++] = v_up_right;


        //vertices.push_back(v_down_right);
        //vertices.push_back(v_up_right);
        //vertices.push_back(v_down_left);
        vertices[vertices_index++] = v_down_right;
        vertices[vertices_index++] = v_up_right;
        vertices[vertices_index++] = v_down_left;


        /*
         every UV is basically in range [0 1] and it denotes the coordinates to sample
         inside the Texture, every (ASCII) character has uv_x = (c%16) /16.f and uv_y = (c/16)/16.f
         now, we must have the UV-coordinates of every letter in our Texture atlas, and every letter
         in our atlas is composed of 4 points (it's a QUAD) so we need to find them:
        */
        char letter = text[i]; //NOTE(ilias): getting the last byte, which is its ASCII encoding value
        f32 uv_x = (letter % 16) / 16.f;
        f32 uv_y = 1 - (letter / 16) / 16.f - 1/16.f; //NOTE(ilias): but why????
        vec2 uv_down_left = {uv_x, uv_y};
        vec2 uv_down_right = {uv_x+1.0f/16.0f, uv_y};
        vec2 uv_up_right = {uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f)};
        vec2 uv_up_left = {uv_x, (uv_y + 1.0f/16.0f)};

        //uvs.push_back(uv_up_left);
        //uvs.push_back(uv_down_left);
        //uvs.push_back(uv_up_right); 
        uvs[uvs_index++] = uv_up_left;
        uvs[uvs_index++] = uv_down_left;
        uvs[uvs_index++] = uv_up_right;


        //uvs.push_back(uv_down_right);
        //uvs.push_back(uv_up_right);
        //uvs.push_back(uv_down_left);
        uvs[uvs_index++] = uv_down_right;
        uvs[uvs_index++] = uv_up_right;
        uvs[uvs_index++] = uv_down_left;
    }
    glBindBuffer(GL_ARRAY_BUFFER, f->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices_index*sizeof(vec2), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, f->uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, uvs_index * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);


   
    use_shader(&f->s);

    //setFloat(&f->s, "time", ((float)time(NULL)) / (float)FLT_MAX);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, f->tex.id); 
    setInt(&f->s, "window_width", global_platform.window_width);
    setInt(&f->s, "window_height", global_platform.window_height);

    glBindVertexArray(f->VAO);

    glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, f->vertex_buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, f->uv_buffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

    glDrawArrays(GL_TRIANGLES, 0, vertices_index);
    glBindVertexArray(0);
}

extern i32 score;
static void 
print_debug_info(BitmapFont *bmf)
{
    glDepthFunc(GL_ALWAYS);
    f32 frequency = 1.f;
//NOTE(ilias): make frequency stuff for all platforms!
#if (_WIN32)
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    frequency = (f32)freq.QuadPart;
#endif
    char string[9];
    sprintf(string, "%iX%i", global_platform.window_width, global_platform.window_height);
    print_text(bmf,string, 0,50, 40);
    sprintf(string, "time: %.2f", global_platform.current_time);
    print_text(bmf,string, 0,100, 40);
    sprintf(string, "fps: %.2f",frequency/ (10000000.f*global_platform.dt));
    print_text(bmf,string, 0,150, 40);
    sprintf(string, "GL: %.5s",glGetString(GL_VERSION)); 
    print_text(bmf,string, 0,200, 40);
      glDepthFunc(GL_LESS);
}

#endif
