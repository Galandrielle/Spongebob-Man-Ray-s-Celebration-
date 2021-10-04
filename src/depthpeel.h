#ifndef DEPTHPEEL_H
#define DEPTHPEEL_H
#include "tools.h"
#include "platform.h"
#include "fbo.h"
#include "quad.h"




extern void  render_scene(void);

OpenGLFBO front;
OpenGLFBO back;
//whether we are CURRENTLY drwing
//in front or back buffer
b32 rendering_front;

static void 
init_depth_peel(void)
{
    front = init_fbo(global_platform.window_width, global_platform.window_height, FBO_COLOR_0 | FBO_DEPTH);
    back = init_fbo(global_platform.window_width, global_platform.window_height, FBO_COLOR_0 | FBO_DEPTH);
    //shader_load(&depthpeel_shader,"../assets/shaders/depthpeel_quad.vert","../assets/shaders/depthpeel_quad.frag");
};

static void 
render_depth_peel(void)
{
    GLuint last_fbo_bound;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_fbo_bound);


    //I dont clear the framebuffers after I draw em
    rendering_front = TRUE; 
    bind_fbo(&front);
    render_scene();
    rendering_front = FALSE;
    bind_fbo(&back);
    render_scene();
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    copy_fbo_contents(back.fbo,0);
    //blend_fbo_contents(front.fbo,0);
    screen_quad.texture = (Texture){front.color_attachments[0], front.w, front.h};
    render_fullscreen_quad(&screen_quad);
    bind_fbo(last_fbo_bound);
}

//TODO: resize should happen only when it is needed!
//this is very very slow.
static void 
clear_depth_peel_fbos(void)
{
    //fbo 0 probably
    GLuint last_fbo_bound;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_fbo_bound);


    //resize and clear the front fbo
    rendering_front = TRUE;
    resize_fbo(&front, global_platform.window_width, global_platform.window_height, FBO_COLOR_0 | FBO_DEPTH);
    clear_fbo(&front);

    //resize and clear the back fbo
    rendering_front = FALSE;
    resize_fbo(&back, global_platform.window_width, global_platform.window_height, FBO_COLOR_0 | FBO_DEPTH);
    clear_fbo(&back);

    bind_fbo(last_fbo_bound);
}

#endif
