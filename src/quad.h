#ifndef QUAD_H
#define QUAD_H
#include "platform.h"
#define SHADER_INCLUDE
#include "shader.h"
#include "texture.h"

typedef struct Quad
{
    Shader shader;
    Texture texture;
    GLuint VAO;
}Quad;


static f32 quad_vertices[] = {
     // positions          // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,    // top left
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
};


//vertices of a full screen quad
static f32 fs_quad_verts[] = { 
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
};
static Quad screen_quad;

static void 
init_quad(Quad *q, char *tex_name)
{
    GLuint VBO;
    shader_load(&q->shader,"../assets/shaders/quad_dp.vert", "../assets/shaders/quad_dp.frag");
    if (!load_texture(&q->texture,tex_name))
        memcpy(infoLog, "texture not found", 18); //see? this is how we stop the program because of a fatal error
    glGenVertexArrays(1, &q->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(q->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,5 * sizeof(float),(void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

static void 
init_fullscreen_quad(Quad *q, char *tex_name)
{
    shader_load(&q->shader,"../assets/shaders/fullscreen_tex.vert", "../assets/shaders/fullscreen_tex.frag");
    if (!load_texture(&q->texture,tex_name))
        memcpy(infoLog, "texture not found", 18);
    //we generate vertex buffers
    GLuint VBO;
	glGenVertexArrays(1, &q->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(q->VAO);

    //we pass our data to the vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fs_quad_verts), fs_quad_verts, GL_STATIC_DRAW);
    //we interpret the data with the vao
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}
static void 
render_fullscreen_quad_opacity(Quad *q, f32 opacity)
{
    //glDisable(GL_DEPTH_TEST);
    //glDepthFunc(GL_ALWAYS);
    glBindVertexArray(q->VAO);
    use_shader(&q->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, q->texture.id);
    setInt(&q->shader, "sampler", 0);
    setFloat(&q->shader, "opacity", opacity);
    glDrawArrays(GL_TRIANGLES,0, 6); 
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glBindVertexArray(0);
}


static void 
render_fullscreen_quad(Quad *q)
{
    //glDisable(GL_DEPTH_TEST);
    //glDepthFunc(GL_ALWAYS);
    glBindVertexArray(q->VAO);
    use_shader(&q->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, q->texture.id);
    setInt(&q->shader, "sampler", 0);
    setFloat(&q->shader, "opacity", 1.f);
    glDrawArrays(GL_TRIANGLES,0, 6); 
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glBindVertexArray(0);
}

static void 
render_quad(Quad *q)
{
    mat4 mvp = m4d(1.f);//if you want you can render with a MVP matrix
    use_shader(&q->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, q->texture.id);
    setMat4fv(&q->shader, "MVP", (float*)mvp.elements);
    glBindVertexArray(q->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
static void 
render_quad_mvp(Quad* q, mat4 mvp)
{
    use_shader(&q->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, q->texture.id);
    setInt(&q->shader, "sampler", 0);
    setMat4fv(&q->shader, "MVP", (float*)mvp.elements);
    glBindVertexArray(q->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


#include "fbo.h"
extern OpenGLFBO front;
extern b32 rendering_front;
static void 
render_quad_mvp_dp(Quad* q, mat4 mvp)
{
    use_shader(&q->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, q->texture.id);
    setInt(&q->shader, "sampler", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, front.depth_attachment);
    setInt(&q->shader, "depth_buffer", 1);
    setMat4fv(&q->shader, "MVP", (float*)mvp.elements);
    if (rendering_front)
        setInt(&q->shader, "peel", 0);
    else
        setInt(&q->shader, "peel",1);
    setInt(&q->shader, "window_width", global_platform.window_width);
    setInt(&q->shader, "window_height", global_platform.window_height);
    glBindVertexArray(q->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

#endif
