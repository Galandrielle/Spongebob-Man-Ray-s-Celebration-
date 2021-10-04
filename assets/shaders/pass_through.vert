#version 460

in vec4 vertex_pos;

smooth out vec4 frag_pos;

void main()
{
    frag_pos = vertex_pos;
    gl_Position = vertex_pos;
}
