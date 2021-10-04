#ifndef TEXTURE_H
#define TEXTURE_H

#include "tools.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ext/stb_image.h"
#include "ext/stb_image_write.h"



typedef struct Texture 
{
    GLuint id;
    u32 width;
    u32 height;
}Texture;
static b32 load_texture_data(Texture* tex,f32 *data)
{
    b32 result = 0;
    //glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //TODO FIX
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //TODO FIX
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = global_platform.window_width;
    int height = global_platform.window_height;
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        result = 1;
    }
    //stbi_image_free(data);
    tex->width = width;
    tex->height = height;
    return result;
}


static b32 load_texture(Texture* tex,const char *filename)
{
    b32 result = 0;
    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //TODO FIX
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //TODO FIX
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width = 0, height = 0, nrChannels;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels,STBI_rgb_alpha);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        result = 1;
    }
    //stbi_image_free(data);
    tex->width = width;
    tex->height = height;
    return result;
}

typedef struct Image 
{
    unsigned char *data;
    i32 width;
    i32 height;
}Image;

static Image 
load_image_bytes(const char *filename)
{
    Image res;
    i32 width = 0, height = 0, nrChannels;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels,STBI_rgb_alpha);
    res.data = data;
    res.width = width;
    res.height = height;
    return res;
}

static void
write_texture2D_to_disk(Texture *tex)
{
   GLfloat *pixels = (GLfloat*)ALLOC(sizeof(f32) * 3 * tex->width* tex->height);   
   glBindTexture(GL_TEXTURE_2D,tex->id);
   glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);
   ppm_save_pixels(tex->width, tex->height,pixels);
}

/* example of changing some texture with the use of OpenGL 4.2 Images

    shader_load(&image_shader,"../assets/shaders/image_demo.vert", "../assets/shaders/image_demo.frag");
    {
        glBindImageTexture(3, q.texture.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
        use_shader(&image_shader);
        setFloat(&image_shader, "time", global_platform.current_time);
        setInt(&image_shader, "width", q.texture.width);
        setInt(&image_shader, "height", q.texture.height);
        glDrawArrays(GL_POINTS, 0, q.texture.width*q.texture.height);//we launch one GPU thread per pixel
        // make sure all computations are done, before we do the next pass, with a barrier.
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        write_texture2D_to_disk(&q.texture.id);
    }

*/


#endif
