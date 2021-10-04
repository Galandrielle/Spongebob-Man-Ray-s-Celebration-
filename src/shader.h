#ifndef SHADER_H
#define SHADER_H

#include "platform.h"
#include "tools.h"
#include <stdlib.h>
#include <stdio.h>

#if !defined(SHADER_INCLUDE)
#define SHADER_INCLUDE 0
#endif



typedef struct Shader
{
    GLuint ID;   
    const char* vertex_str;
    const char* fragment_str;
}Shader;

char * 
shader_read_whole_file(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = (char*)ALLOC(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);
    string[fsize] = 0; 

//make the char * have the includes source too inside
//also TODO: fix this shit, most tangled mess ever
//if you dont need includes delete this, it ll make your life easier
#if 0

    char **include_paths = NULL;
    char *inc_index;
    while (TRUE)
    {
        if (inc_index = strstr(string, "#include"))
        {
            i32 index = (u32)inc_index - (u32)string;
            //comment out the include so it will compile fine!
            //string[index] = '/';
            //string[index+1] = '/';

            //index of "someshader.frag"
            i32 string_start = find_char_in_string(string, index, '\"') + 1;
            i32 string_end = find_char_in_string(string, (u32)string_start + 1, '\"');
            char *include_name = (char *)ALLOC(sizeof(char)*(string_end - string_start));
            //makes include_name have the include path as value
            memcpy(include_name, (char*)(string + string_start), string_end - string_start); 
            include_name[string_start-string_end - 2] = '\0';

            buf_push(include_paths, include_name);
        }else
            break;
    }

    //size of each shader source
    u32 *include_sizes = NULL;
    for (int i = 0; i < buf_len(include_paths); ++i)
    {
       buf_push(include_sizes,get_file_size(include_paths[i])); 
    }
    //all the shader sources included
    char **include_sources = NULL; 
    for (int i = 0; i < buf_len(include_paths); ++i)
    {
       buf_push(include_sources,shader_read_whole_file(include_paths[i])); 
    }

    u32 total_size = fsize;
    for (i32 i = 0; i < buf_len(include_paths);++i)
        total_size += include_sizes[i];

    char *whole_shader = (char *)ALLOC(total_size * sizeof(char));
    
#endif

    return (char*)string;
}



static void use_shader(Shader* shader)
{
    glUseProgram(shader->ID);
}

static GLuint load_shader_from_strings (const char * vertex_str, const char * fragment_str)
{ 
    GLuint ID;
    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
       
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_str, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    }
      
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_str, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    }
      
    
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        //glGetProgramInfoLog(ID, 512, NULL, infoLog);
    }
      
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return ID;
}
static void shader_load (Shader* s, const char * vertex_path, const char * fragment_path)
{
    s->vertex_str = vertex_path;
    s->fragment_str = fragment_path;
    const char* vs = shader_read_whole_file(vertex_path);
    const char* fs = shader_read_whole_file(fragment_path); 
    s->ID = load_shader_from_strings(vs,fs);

}

static void reload_shader_from_files( GLuint* program, const char* vertex_shader_filename, const char* fragment_shader_filename ) {
  assert( program && vertex_shader_filename && fragment_shader_filename );
  Shader new_shader;
  shader_load(&new_shader,vertex_shader_filename, fragment_shader_filename );
  if ( new_shader.ID ) {
    glDeleteProgram( *program );
    *program = new_shader.ID;
  }
}
static void 
setBool(Shader* shader, const char *name, b8 value)
{         
    glUniform1i(glGetUniformLocation(shader->ID, name), (int)value); 
}
static void 
setInt(Shader* shader, const char *name, i32 value)
{ 
    glUniform1i(glGetUniformLocation(shader->ID, name), value); 
}
static void 
setFloat(Shader * shader, const char *name, f32 value)
{ 
    glUniform1f(glGetUniformLocation(shader->ID, name), value); 
} 
static void
setVec3(Shader * shader, const char *name, vec3 value)
{ 
    glUniform3f(glGetUniformLocation(shader->ID, name), value.x,value.y,value.z); 
} 

static void
setVec4(Shader * shader, const char *name, vec4 value)
{ 
    glUniform3f(glGetUniformLocation(shader->ID, name), value.x,value.y,value.z, value.w); 
} 
static void 
setMat4fv(Shader * shader, const char *name, f32* value)
{ 
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name),1,GL_FALSE, value);  //NOTE(ilias): check
}
#endif
