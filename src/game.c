#include "platform.h"
#include "tools.h"
#include "quad.h" 
#include "camera.h"
#include "objloader.h"
#include "model.h"
#include "text.h"
#include "collada_parser.h"
#include "skybox.h"

static Quad q;
static Camera cam;
static BitmapFont bmf;
static Skybox skybox;

static mat4 view;
static mat4 proj;
static vec4 background_color;
static i32 state;

static Model model;
static MeshInfo mesh;

static Model star_model;
static MeshInfo star_mesh;

static Model chest_model;
static MeshInfo chest_mesh;

static Model sign_model;
static MeshInfo sign_mesh;


static Animator animator;
static Quad compass[10];
static u32 current_state;
static u32 score;
static f32 score_size;
static u32 can_screenshot;
static u32 screenshot_timer;
static Quad screenshot_to_render;
static f32 screenshot_opacity;
static Animation anims[4];
static i32 moves[] = {
  1,2,3,1,3,
  2,1,0,1,2,
  1,3,2,1,2,
  1,2,3,2,1,
  1,2,3,0,1,
  3,1,3,1,3,
  0,1,2,3,2,
  2,1,0,1,2,
};

static void ppm_save_pixels3(LPVOID lpParam)
{
            PMYDATA real_data = (PMYDATA)(lpParam);
            u8 *pixels = real_data->data; 
            stbi_write_png("score.png", global_platform.window_width, global_platform.window_height, 3, pixels,sizeof(u8)* 3 * global_platform.window_width);
}

static void 
init(void)
{
    init_camera(&cam);
    init_text(&bmf, "../assets/BMF.png");
    char *skybox_faces[6] = {"../assets/env/rt.png", "../assets/env/lf.png", "../assets/env/dn.png",
        "../assets/env/up.png", "../assets/env/bk.png", "../assets/env/ft.png" };
    init_skybox(&skybox, skybox_faces);

 
    animator = init_animator(str(&global_platform.frame_storage,"../assets/man.png"), 
            str(&global_platform.frame_storage,"../assets/left.dae"), str(&global_platform.frame_storage,"../assets/up.dae"));  
    background_color = v4(0.4,0.7,0.7,1.f);

    //init compass
    {
      init_quad(&compass[0], "../assets/compass2/compass.png");
      init_quad(&compass[1], "../assets/compass2/down_arrow.png");
      init_quad(&compass[2], "../assets/compass2/up_arrow.png");
      init_quad(&compass[3], "../assets/compass2/left_arrow.png");
      init_quad(&compass[4], "../assets/compass2/right_arrow.png");
      init_quad(&compass[5], "../assets/compass2/down_arrow_blue.png");
      init_quad(&compass[6], "../assets/compass2/up_arrow_blue.png");
      init_quad(&compass[7], "../assets/compass2/left_arrow_blue.png");
      init_quad(&compass[8], "../assets/compass2/right_arrow_blue.png");
      init_quad(&compass[9], "../assets/compass2/compass2.png");
      init_fullscreen_quad(&screenshot_to_render, "../assets/BMF.png");
      current_state = 6;
      state = 1;
      score = 0;
      score_size = 50.f;
    }
    //terrain initialization
    {
        mesh = load_obj("../assets/cloud/cloud.obj");
        //mesh = load_obj("../assets/utah_teapot.obj");
        init_model_textured_basic(&model, &mesh);
        load_texture(&(model.diff),"../assets/cloud/cloud.png");
    }
    {
        star_mesh = load_obj("../assets/star/star.obj");
        //mesh = load_obj("../assets/utah_teapot.obj");
        init_model_textured_basic(&star_model, &star_mesh);
        load_texture(&(star_model.diff),"../assets/star/star.png");
    }
    {
        chest_mesh = load_obj("../assets/chest/chest.obj");
        //mesh = load_obj("../assets/utah_teapot.obj");
        init_model_textured_basic(&chest_model, &chest_mesh);
        load_texture(&(chest_model.diff),"../assets/chest/chest.png");
    }
   {
        sign_mesh = load_obj("../assets/sign.obj");
        //mesh = load_obj("../assets/utah_teapot.obj");
        init_model_textured_basic(&sign_model, &sign_mesh);
        load_texture(&(sign_model.diff),"../assets/sign.png");
    }

    //init animation
     can_screenshot = 0;
    screenshot_timer = 50.f;
    screenshot_opacity = 0.f;
}



static void 
update(void) {
    update_cam(&cam);
    view = get_view_mat(&cam);
    proj = perspective_proj(45.f,global_platform.window_width / (f32)global_platform.window_height, 0.1f,300.f); 
    current_state = 5 + moves[(int)(global_platform.current_time - 0.1) % 40];
    screenshot_timer -= global_platform.dt;
    if (screenshot_timer < 0.f)
      screenshot_timer = 0;
   screenshot_opacity -= 0.07;
   if(screenshot_opacity < 0.f)
     screenshot_opacity = 0.f;
}
static void 
render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(background_color.x, background_color.y, background_color.z,background_color.w);
    if (global_platform.key_pressed[KEY_DOWN] && state != 0)
    {
        *animator.anim = read_collada_animation(str(&global_platform.permanent_storage,"../assets/down.dae"));
        state = 0;
        //*animator.anim = anims[state];
        animator.blend_percentage = 1.f;
        animator.blend_time = 0.2f;
    }
    if (global_platform.key_pressed[KEY_UP] && state != 1)
    {
        //JointKeyFrame *prev_pose = animator.prev_pose;
        *animator.anim = read_collada_animation(str(&global_platform.permanent_storage,"../assets/up.dae"));
        state = 1;
        //*animator.anim = anims[state];
        animator.blend_percentage = 1.f;
        animator.blend_time = 0.2f;
    }
    if (global_platform.key_pressed[KEY_LEFT] && state != 2)
    {
        *animator.anim = read_collada_animation(str(&global_platform.permanent_storage,"../assets/left.dae"));
        state = 2;
        //*animator.anim = anims[state];
        animator.blend_percentage = 1.f;
        animator.blend_time = 0.2f;
    }
    if (global_platform.key_pressed[KEY_RIGHT] && state != 3)
    {
        *animator.anim = read_collada_animation(str(&global_platform.permanent_storage,"../assets/right.dae"));
        state = 3;
        //*animator.anim = anims[state];
        animator.blend_percentage = 1.f;
        animator.blend_time = 0.2f;
    }


    render_skybox(&skybox);
    model.position = v3(0,0,0);
    render_model_textured_basic(&model, &proj, &view);
    model.position = v3(10,0,0);
    render_model_textured_basic(&model, &proj, &view);
    model.position = v3(-10,0,0);
    render_model_textured_basic(&model, &proj, &view);
    model.position = v3(-22,0,0);
    render_model_textured_basic(&model, &proj, &view);
    model.position = v3(22,0,0);
    render_model_textured_basic(&model, &proj, &view);

    star_model.position = v3(10 + cos(global_platform.current_time) * 50,10 + sin(global_platform.current_time)*50, -50 + sin(global_platform.current_time) *50);
    render_model_textured_basic(&star_model, &proj, &view);
    star_model.position = v3(10 + cos(global_platform.current_time) * 70,10 + sin(global_platform.current_time)*30, -50 + sin(global_platform.current_time) *20);
    render_model_textured_basic(&star_model, &proj, &view);
    star_model.position = v3(10 -cos(global_platform.current_time) * 80,10 + sin(global_platform.current_time)*40, -50 + sin(global_platform.current_time) *30);
    render_model_textured_basic(&star_model, &proj, &view);
    star_model.position = v3(10 -cos(global_platform.current_time) * 100,10 - sin(global_platform.current_time)*40, -50 + sin(global_platform.current_time) *60);
    render_model_textured_basic(&star_model, &proj, &view);
 
    chest_model.position = v3(15,0,0);
    chest_model.scale = v3(2,2,2);
    render_model_textured_basic(&chest_model, &proj, &view);

    chest_model.position = v3(-15,0,0);
    render_model_textured_basic(&chest_model, &proj, &view);

    sign_model.position = v3(-5,0,10);
    sign_model.scale = v3(1.f/10,1.f/10,1.f/10);
    render_model_textured_basic(&sign_model, &proj, &view);

   

 
    update_animator(&animator);
    render_animated_model(&animator.model, &anim_shader, proj, view);
    //render_animated_model_static(&animator.model, &anim_shader, proj, view, v3(15,0,0), 1.f);
    //render_animated_model_static(&animator.model, &anim_shader, proj, view, v3(-15,0,0), -1.f);
    if (equalf(screenshot_timer,0,0.0001f) && score_size > 51.f) 
    {
      can_screenshot = 1;
      render_quad_mvp(&compass[9],mul_mat4(proj, mul_mat4(view, mul_mat4(translate_mat4(add_vec3(cam.pos, v3(0,-0.5,-2.f))), scale_mat4(v3(0.4,0.4,0.4))))));
    }else{
      render_quad_mvp(&compass[0],mul_mat4(proj, mul_mat4(view, mul_mat4(translate_mat4(add_vec3(cam.pos, v3(0,-0.5,-2.f))), scale_mat4(v3(0.4,0.4,0.4))))));
      can_screenshot = 0;
    }

    //glDepthMask(GL_FALSE);
    render_quad_mvp(&compass[state+1],mul_mat4(proj, mul_mat4(view, mul_mat4(translate_mat4(add_vec3(cam.pos, v3(0,-0.5,-1.9999f))), scale_mat4(v3(0.4,0.4,0.4))))));
   
    if (current_state != state+5)
    {
      load_texture(&(star_model.diff),"../assets/star/star_sad.png");
      render_quad_mvp(&compass[current_state],mul_mat4(proj, mul_mat4(view, mul_mat4(translate_mat4(add_vec3(cam.pos, v3(0,-0.5,-1.9998f))), scale_mat4(v3(0.4,0.4,0.4))))));
      score_size -= 0.2;
      score_size = max(50,min(100, score_size));
    }
    else
    {
      load_texture(&(star_model.diff),"../assets/star/star.png");
      score += 5;
      score_size+= max(0.1,global_platform.dt * (score_size/10));
    }
    char string[21];
    sprintf(string, "SCORE: ",score); 
    print_text(&bmf,string, global_platform.window_width - 50 *5,0, 50);
    sprintf(string, "%d",score); 
    print_text(&bmf,string, global_platform.window_width  + 50 * 2,0, (i32)score_size);

    //rendering the health bar
    for (i32 i = 0; i < 16; ++i)
      string[i] = (char)8;
    print_text(&bmf,string,0,global_platform.window_height*2 - 50, 50);
    for (i32 i = 0; i < (int)(((f32)global_platform.permanent_storage.current_offset / (f32)global_platform.permanent_storage.memory_size)* 16.5f); ++i)
      string[i] = (char)10;
    print_text(&bmf,string,0,global_platform.window_height*2 - 50, 50);

    //glDepthMask(GL_TRUE);

    if (global_platform.key_down[KEY_TAB])
        print_debug_info(&bmf);
  if ((global_platform.key_pressed[KEY_K] || global_platform.key_pressed[KEY_SPACE]) && can_screenshot)
    {
          screenshot_opacity = 0.7f;
          can_screenshot = 0;
          screenshot_timer = 500.f;

          f32 *pixels2 = (f32*)ALLOC(sizeof(f32) * 3 * global_platform.window_width* global_platform.window_height); 
          glReadPixels(0, 0, global_platform.window_width,global_platform.window_height,GL_RGB, GL_FLOAT, pixels2);
          u8 *pixels = (u8*)ALLOC(sizeof(u8) * 3 * global_platform.window_width* global_platform.window_height); 
          u8 *pixels_new = (u8*)ALLOC(sizeof(u8) * 3 * global_platform.window_width* global_platform.window_height); 
          ///*
          {
            i32 new_i = 0;
            glReadPixels(0, 0, global_platform.window_width,global_platform.window_height,GL_RGB, GL_UNSIGNED_BYTE, pixels);
            i32 i,j;
            for (j = global_platform.window_height-1; j >=0; --j)
            {
                for (i = 0; i < global_platform.window_width; ++i)
                {
                    i32 index = global_platform.window_width * 3 *j + 3 * i;
                    u8 cmp[3];
                    pixels_new[new_i++]= (u8)(pixels[index]);
                    pixels_new[new_i++]= (u8)(pixels[index+1]);
                    pixels_new[new_i++]= (u8)(pixels[index+2]);
              }
            }
            //stbi_write_png("score.png", global_platform.window_width, global_platform.window_height, 3, pixels_new,sizeof(u8)* 3 * global_platform.window_width);
          }

          //*/

          for (int i = 0; i < 3 * global_platform.window_width * global_platform.window_height; ++i)
              pixels2[i] =  pixels2[i] / 1.5f;
          load_texture_data(&screenshot_to_render.texture,pixels2);
          i64 iden;
          DATA data_for_function = (DATA){global_platform.window_width, global_platform.window_height, pixels_new};
          HANDLE thread2 = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            ppm_save_pixels3,       // thread function name
            &data_for_function,          // argument to thread function 
            0,                      // use default creation flags 
            &iden);   // returns the thread identifier 

          //ppm_save_pixels( global_platform.window_width, global_platform.window_height, pixels);
          //free(pixels);
          can_screenshot = 0;
    }
    render_fullscreen_quad_opacity(&screenshot_to_render,screenshot_opacity); 


}

