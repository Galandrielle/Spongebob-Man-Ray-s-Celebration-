#ifndef SKYBOX_H
#define SKYBOX_H


#include "tools.h"

extern mat4 proj, view;

static u32 
load_cubemap(char ** faces)
{
    u32 textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    //stbi_set_flip_vertically_on_load(TRUE);
    i32 width, height, nrChannels;
    for (u32 i = 0; i < 6; i++)
    {
        unsigned char *data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else
        {
            sprintf(infoLog, "cubemap at: %s failed to load!\n", faces[0]);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


typedef struct Skybox {
	
	u32 vao;
	u32 vbo;
    u32 tex_id;
    char **faces;
    Shader shader;
}Skybox;

static void 
load_skybox(Skybox *skybox,const char**faces) {
    skybox->faces = faces;
    skybox->tex_id = load_cubemap(faces);
    //make a cubemap_texture 
    //attach the images
    // render
}

static void
init_skybox(Skybox* skybox, char**faces) {
    shader_load(&skybox->shader,"../assets/shaders/skybox_rendering.vert", "../assets/shaders/skybox_rendering.frag");
    load_skybox(skybox,faces);
    f32 skybox_vertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &skybox->vao);
    glGenBuffers(1, &skybox->vbo);
    glBindVertexArray(skybox->vao);
    glBindBuffer(GL_ARRAY_BUFFER, skybox->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);	
    glBindVertexArray(0);
}

static void 
render_skybox(Skybox* skybox) {
    use_shader(&skybox->shader);
    //this render must be done at the beginning of the scene rendering process
    //because we disabled depth testing and basically only the color buffer gets updated
    glDepthMask(GL_FALSE);
    glBindVertexArray(skybox->vao);
    setMat4fv(&skybox->shader, "uniform_projection_matrix", (float*)proj.elements);
    setMat4fv(&skybox->shader, "uniform_view_matrix", (float*)view.elements);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->tex_id);
    setInt(&skybox->shader, "skybox", 0);
    //setMat4fv(&skybox->shader, "MVP", (float*)(mul_mat4(projection,view).elements));

    glDrawArrays(GL_TRIANGLES, 0,36);
    glDepthMask(GL_TRUE);
}

#endif
