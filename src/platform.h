#ifndef PLATFORM_H
#define PLATFORM_H

//these are here just for compatibility
#include <windows.h>
#include <gl/gl.h>
#include "ext/glext.h"
#include "ext/wglext.h"

#include "tools.h"
#define KEY_MAX 100

//errors are written in infoLog (with memset?), the program will crash and produce an error box
//its implementation is in win32_main.cpp
extern char infoLog[512]; 

typedef struct Platform
{
    i32 window_width; //width of window (width border)
    i32 window_height; //height of window (with border)
    b32 exit; //whether we should exit
    b32 vsync; //whether the app has vsync (capped FPS.. kinda)
    b32 fullscreen; //whether the app is in fullscreen
    b32 initialized; //whether the app is initialized
    b32 window_resized; //whether the window was resize last frame
    f32 target_fps; //in how many frames should the app run
    f32 current_time; //current time in milliseconds

    f32 mouse_x; //mouse x pos wrt top left corner
    f32 mouse_y; //mouse y coordinate wrt top left corner
    b8 left_mouse_down; //whether LMB is down 
    b8 right_mouse_down; //whether RMB is down
    b8 key_down[KEY_MAX]; //whether a key is down
    b8 key_pressed[KEY_MAX]; //whether a key was JUST pressed
    i32 last_key; //the last key pressed
    b32 keyboard_used; //whether keyboard is used..
    f32 dt; //time between frames in milliseconds


    Arena permanent_storage; //an arena to store permanent data, never flushed
    Arena frame_storage; //an arena to store frame data, flushed EVERY frame
}Platform;

//this global_platform variable is the platform of the engine
//whatever you want to query or change, the global platform
//is the guy you need
extern Platform global_platform;

//if you want to query whether, say, a key is pressed, you write key_pressed[KEY_K], it returns 0
//if its not pressed, some positive number if pressed
enum keyboard_keys
{
    KEY_A = 1, // NOTE: Starts at 1 so that 0 can represent no input
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    KEY_NUMPAD_0,
    KEY_NUMPAD_1,
    KEY_NUMPAD_2,
    KEY_NUMPAD_3,
    KEY_NUMPAD_4,
    KEY_NUMPAD_5,
    KEY_NUMPAD_6,
    KEY_NUMPAD_7,
    KEY_NUMPAD_8,
    KEY_NUMPAD_9,
    KEY_NUMPAD_MULTIPLY,
    KEY_NUMPAD_ADD,
    KEY_NUMPAD_SUBTRACT,
    KEY_NUMPAD_DECIMAL,
    KEY_NUMPAD_DIVIDE,

    KEY_LEFT,
    KEY_UP,
    KEY_RIGHT,
    KEY_DOWN,

    KEY_BACKSPACE,
    KEY_TAB,
    KEY_CTRL,
    KEY_RETURN,
    KEY_SPACE,
    KEY_LSHIFT,
    KEY_RSHIFT,
    KEY_LCONTROL,
    KEY_RCONTROL,
    KEY_ALT,
    KEY_LSUPER,
    KEY_RSUPER,
    KEY_CAPSLOCK,
    KEY_ESCAPE,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
    KEY_HOME,
    KEY_END,
    KEY_INSERT,
    KEY_DELETE,
    KEY_PAUSE,
    KEY_NUMLOCK,
    KEY_PRINTSCREEN,

    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
};

#if _WIN32
    #define GLProc(type, name) PFNGL##type##PROC name;
#endif

//these are the available MODERN openGL functions you can use.
//to load more make the declaration extern GLProc(, and implement in win32_main.c
//extern GLProc( GLProc(GENBUFFERS, glGenBuffers);


extern GLProc( GENBUFFERS, glGenBuffers);
extern GLProc( BINDBUFFER, glBindBuffer);
extern GLProc( DRAWBUFFERS, glDrawBuffers);
extern GLProc( USEPROGRAM, glUseProgram);
extern GLProc( SHADERSOURCE, glShaderSource);
extern GLProc( COMPILESHADER, glCompileShader);
extern GLProc( GETSHADERIV, glGetShaderiv);
extern GLProc( MAPBUFFERRANGE, glMapBufferRange);
extern GLProc( MAPBUFFER, glMapBuffer);
extern GLProc( CREATESHADER, glCreateShader);
extern GLProc( GETSHADERINFOLOG, glGetShaderInfoLog);
extern GLProc( GETPROGRAMINFOLOG, glGetProgramInfoLog);
extern GLProc( CREATEPROGRAM, glCreateProgram);
extern GLProc( ATTACHSHADER, glAttachShader);
extern GLProc( DELETESHADER, glDeleteShader);
extern GLProc( DELETEPROGRAM, glDeleteProgram);
extern GLProc( LINKPROGRAM, glLinkProgram);
extern GLProc( GETPROGRAMIV, glGetProgramiv);
extern GLProc( UNIFORM1I, glUniform1i);
extern GLProc( UNIFORM3F, glUniform3f);
extern GLProc( UNIFORM1IV, glUniform1iv);
extern GLProc( UNIFORM2FV, glUniform2fv);
extern GLProc( UNIFORM1F, glUniform1f);
extern GLProc( ACTIVETEXTURE, glActiveTexture);
extern GLProc( VERTEXATTRIBDIVISOR, glVertexAttribDivisor);
extern GLProc( GETUNIFORMLOCATION, glGetUniformLocation);
extern GLProc( GENVERTEXARRAYS, glGenVertexArrays);
extern GLProc( DRAWELEMENTSINSTANCED, glDrawElementsInstanced);
extern GLProc( DRAWARRAYSINSTANCED, glDrawArraysInstanced);
extern GLProc( BINDVERTEXARRAY, glBindVertexArray);
extern GLProc( UNIFORMMATRIX4FV, glUniformMatrix4fv);
extern GLProc( BUFFERDATA, glBufferData);
extern GLProc( VERTEXATTRIBPOINTER, glVertexAttribPointer);
extern GLProc( VERTEXATTRIBIPOINTER, glVertexAttribIPointer);
extern GLProc( ENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray);
extern GLProc( GENERATEMIPMAP, glGenerateMipmap);
extern GLProc( GENFRAMEBUFFERS, glGenFramebuffers);
extern GLProc( FRAMEBUFFERTEXTURE2D, glFramebufferTexture2D);
extern GLProc( BINDFRAMEBUFFER, glBindFramebuffer);
extern GLProc( CHECKFRAMEBUFFERSTATUS, glCheckFramebufferStatus);
extern GLProc( BINDRENDERBUFFER, glBindRenderbuffer);
extern GLProc( RENDERBUFFERSTORAGE, glRenderbufferStorage);
extern GLProc( GENRENDERBUFFERS, glGenRenderbuffers);
extern GLProc( FRAMEBUFFERRENDERBUFFER, glFramebufferRenderbuffer);
extern GLProc( TEXIMAGE3D, glTexImage3D);
extern GLProc( BINDIMAGETEXTURE, glBindImageTexture);
extern GLProc( MEMORYBARRIER, glMemoryBarrier);
extern GLProc( COPYIMAGESUBDATA, glCopyImageSubData);
extern GLProc( BLENDFUNCSEPARATE, glBlendFuncSeparate);
extern GLProc( DELETEFRAMEBUFFERS, glDeleteFramebuffers);
extern GLProc( BLITFRAMEBUFFER, glBlitFramebuffer);
extern GLProc( BINDBUFFERBASE, glBindBufferBase);
extern GLProc( UNMAPBUFFER, glUnmapBuffer);
extern GLProc( GETBUFFERSUBDATA, glGetBufferSubData);
extern GLProc( CLEARBUFFERDATA, glClearBufferData);
extern GLProc( TEXSTORAGE2D, glTexStorage2D);
extern GLProc( BUFFERSUBDATA, glBufferSubData);
extern GLProc( CLEARTEXIMAGE, glClearTexImage);
#endif
