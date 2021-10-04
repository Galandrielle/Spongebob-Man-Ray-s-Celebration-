#ifndef MODEL_H
#define MODEL_H

#include "objloader.h"
#include "tools.h"
#include "platform.h"

/*
NOTE(ilias): OpenGL function pointers needed if
model.h is used in another translation unit
*/
#include "platform.h"

typedef struct Model
{
    GLuint vao;
    MeshInfo *mesh;
    Texture spec;
    Texture diff;
    Shader s;
    vec3 position;
    vec3 scale;
}Model;


typedef struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    f32 shininess;
}Material;
typedef struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}Light;
typedef struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}DirLight;  
typedef struct PointLight {    
    vec3 position;
    
    f32 constant;
    f32 linear;
    f32 quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}PointLight;  

//NOTE(ilias): these are for debug, in case the renderer
//is not available, to render models, push to renderer!

static void 
init_model_textured_basic(Model* m, MeshInfo *mesh)
{
    {
        m->mesh = mesh;
        m->scale = v3(1,1,1);
        //m->position = v3(random01()*3,0,0); 
    }
    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao); GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->vertices_count, &mesh->vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 6));
    glBindVertexArray(0);


    shader_load(&m->s,"../assets/shaders/mesh.vert","../assets/shaders/mesh.frag");
    load_texture(&(m->diff),"../assets/red.png");
    load_texture(&(m->spec),"../assets/white.png");
      
}

static void 
render_model_textured_basic(Model* m,mat4 *proj,mat4 *view)
{
    use_shader(&m->s);
    
    setInt(&m->s, "sampler", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m->diff.id);
    setInt(&m->s, "m.specular", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m->spec.id);
    //mat4 mvp = mul_mat4(mv,translate_mat4(m->position));
    mat4 model = mul_mat4(translate_mat4(m->position),scale_mat4(m->scale));
    setMat4fv(&m->s, "model", (GLfloat*)model.elements);
    setMat4fv(&m->s, "view", (GLfloat*)view->elements);
    setMat4fv(&m->s, "proj", (GLfloat*)proj->elements);

    glBindVertexArray(m->vao);
    glDrawArrays(GL_TRIANGLES,0, m->mesh->vertices_count);
    glBindVertexArray(0);
}

#endif
