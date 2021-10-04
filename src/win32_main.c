#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "platform.h"
Platform global_platform;
//errors are written here, the program will crash and produce an error
char infoLog[512]; 
static HDC global_device_context;
static HWND WND;
#include <windows.h>
#include "game.c"
#include "win32_opengl.c"
#include "tools.h"

//these are sindows specific stuff because i didnt want to include a whole header :P
int SND_SYNC = 0000,
SND_ASYNC = 0x0001,
SND_NODEFAULT = 0x0002,
SND_MEMORY = 0x0004,
SND_LOOP = 0x0008,
SND_PURGE = 0x0040,
SND_FILENAME = 0x00020000,
SND_NOSTOP = 0x0010;
static LARGE_INTEGER last_frame, end_frame, frequency;

static LRESULT 
Win32WindowProc(HWND hWnd, UINT message, WPARAM w_param, LPARAM l_param) {

    LRESULT result = {0};
    if (message == WM_SIZE)
        {
           RECT rect;
           GetClientRect(hWnd, &rect);
           //NOTE(ilias): The renderer should handle this, when finished, pass this over!!
           glViewport(0, 0, (GLsizei)rect.right, (GLsizei)rect.bottom); //for some reason this is useless 
           global_platform.window_width = rect.right;
           global_platform.window_height = rect.bottom;
           global_platform.window_resized = 1;
    }else if (message == WM_CLOSE || message == WM_DESTROY || message == WM_QUIT){
        global_platform.exit = 1;
    }else if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP || message == WM_KEYDOWN || message == WM_KEYUP){
        u64 vkey_code = w_param;
        i8 was_down = !!(l_param & (1 << 30));
        i8 is_down = !(l_param & (1 << 31));

        u64 key_input =0;

        if ((vkey_code >= 'A' && vkey_code <= 'Z') || (vkey_code >='0' && vkey_code <= '9')){
            key_input = (vkey_code >='A' && vkey_code <= 'Z') ? KEY_A + (vkey_code - 'A') : KEY_0 + (vkey_code - '0');
        }else {
            if (vkey_code == VK_F4){
                key_input = KEY_F4;
            }else if (vkey_code == VK_SPACE){
                key_input = KEY_SPACE;
            }else if (vkey_code == VK_MENU)
            {
                key_input= KEY_ALT;
            }else if (vkey_code == VK_TAB)
             {
                 key_input = KEY_TAB;
             }
            else if (vkey_code == VK_LEFT)
            {
                key_input = KEY_LEFT;
            }
            else if (vkey_code == VK_RIGHT)
            {
                key_input = KEY_RIGHT;
            }
            else if (vkey_code == VK_UP)
            {
                key_input = KEY_UP;
            }
            else if (vkey_code == VK_DOWN)
            {
                key_input = KEY_DOWN;
            }
             else if (vkey_code == VK_CONTROL)
            {
                key_input = KEY_CTRL;
            }

           //handle more keys
        }
        if (is_down){
           if (!global_platform.key_down[key_input])
           {
               ++global_platform.key_pressed[key_input];
           }else
           {
               global_platform.key_pressed[key_input] = 0;
           }
           ++global_platform.key_down[key_input];
           global_platform.last_key = (i32)key_input;
           if(global_platform.key_down[KEY_ALT] && key_input == KEY_F4)
            {
                global_platform.exit = 1;
            }
        }else 
        {
            global_platform.key_down[key_input] = 0;
            global_platform.key_pressed[key_input] = 0;
        }
    }else if (message == WM_LBUTTONDOWN)
    {
        global_platform.left_mouse_down = 1;
    }else if (message == WM_LBUTTONUP)
    {
        global_platform.left_mouse_down = 0;
    }else if (message == WM_RBUTTONDOWN)
    {
        global_platform.right_mouse_down = 1;
    }else if (message == WM_RBUTTONUP)
    {
        global_platform.right_mouse_down = 0;
    }

    else{
        result = DefWindowProc(hWnd, message, w_param, l_param);
    }
    return result;
}

static void
Win32CleanUpOpenGL(HDC *device_context)
{
    wglMakeCurrent(*device_context, 0);
    wglDeleteContext(win32_opengl_context);
}

static void
Win32OpenGLSetVerticalSync(b32 vsync)
{
    wglSwapIntervalEXT(!!vsync);
}

static void
Win32OpenGLSwapBuffers(HDC device_context)
{
    wglSwapLayerBuffers(device_context, WGL_SWAP_MAIN_PLANE);
}

i32 CALLBACK
WinMain(HINSTANCE Instance,

        HINSTANCE PrevInstance,

        LPSTR CommandLine,

        i32 ShowCode)

{
    WNDCLASS windowClass = {0};
    {
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = (WNDPROC)Win32WindowProc;
        windowClass.hInstance = Instance;
        windowClass.lpszClassName = "WindowClass";
        windowClass.hCursor = LoadCursor(0, IDC_ARROW);

    }
    RegisterClass(&windowClass);
    WND = CreateWindow(
            windowClass.lpszClassName, "SpongeBob: Man Ray's Celebration!",      // window class, title
            WS_OVERLAPPEDWINDOW, // style
            CW_USEDEFAULT,CW_USEDEFAULT,1280,720,//CW_USEDEFAULT,CW_USEDEFAULT,
            NULL, NULL,                 // parent window, menu
            Instance, NULL);           // instance, param
     
    HDC DC = GetDC(WND);        // Device Context

    PlaySound(TEXT("../assets/mix.wav"), NULL,SND_FILENAME | SND_ASYNC | SND_LOOP); 
    //NOTE(ilias): initializing the platform layer
    {
        global_platform.exit = 0;
        global_platform.window_width = 1280;
        global_platform.window_height = 720;
        global_platform.target_fps = 60.f;
        global_platform.current_time = 0.f;
    }

    //implemented in win32_opengl.c
    win32_init_opengl(&DC, Instance); 

    //uncomment this if you want your OGL version as wintext
    //SetWindowText(WND, (LPCSTR)glGetString(GL_VERSION));

    ShowWindow(WND, ShowCode);

    MSG msg;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    //glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);


    //NOTE(ilias): initializing arenas
    {
        u8* permanent_memory = (u8*)malloc(megabytes(16));
        u8* frame_memory = (u8*)malloc(megabytes(64));
        //mem = (void *)((u8*)arena->memory + arena->current_offset); 
        if (permanent_memory == NULL || frame_memory == NULL)memcpy(infoLog, "Not Enough Storage for Arenas", 29);
        global_platform.permanent_storage = arena_init(permanent_memory, megabytes(16));
        global_platform.frame_storage = arena_init(frame_memory, megabytes(64));
     
    }

    //NOTE(ilias): returns counts/sec (constant thru program)
    QueryPerformanceFrequency(&frequency);

    init();
    while (!global_platform.exit){
        QueryPerformanceCounter(&last_frame);
        while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        {
            POINT mouse;
            GetCursorPos(&mouse);
            ScreenToClient(WND, &mouse);
            global_platform.mouse_x = (f32)(mouse.x);
            global_platform.mouse_y = (f32)(mouse.y);
        }
        
        {
            RECT client_rect;
            GetClientRect(WND, &client_rect);
            global_platform.window_width = client_rect.right - client_rect.left;
            global_platform.window_height = client_rect.bottom - client_rect.top;
        }
        update();
        render();
        SwapBuffers(GetDC(WND));

        arena_clear(&global_platform.frame_storage);
        QueryPerformanceCounter(&end_frame);

#if 1
        //NOTE(ilias): wait remaining time for targeted fps! (if you want max speed do #if 0, not recommended)
        i64 frame_count = end_frame.QuadPart - last_frame.QuadPart;
        i64 desired_frame_count = (f32)frequency.QuadPart / global_platform.target_fps;
        i64 counts_to_wait = desired_frame_count - frame_count;

        LARGE_INTEGER begin_wait_time_delta;
        LARGE_INTEGER end_wait_time_delta;
        QueryPerformanceCounter(&begin_wait_time_delta);
        while(counts_to_wait> 0)
        {
            QueryPerformanceCounter(&end_wait_time_delta);
            counts_to_wait -= (end_wait_time_delta.QuadPart - begin_wait_time_delta.QuadPart);
            begin_wait_time_delta = end_wait_time_delta;
        }
        //NOTE(ilias): the real end (after dead wait)
        QueryPerformanceCounter(&end_frame);
#endif
        f32 dt = (end_frame.QuadPart - last_frame.QuadPart)/ (float)frequency.QuadPart;
        global_platform.dt = dt;
        global_platform.current_time += dt;
        global_platform.window_resized = 0;

        if (strlen(infoLog) != 0){
            MessageBox(WND, infoLog, "FATAL ERROR", MB_OK);
            exit(1);
        }

    }
    return 0;
}
