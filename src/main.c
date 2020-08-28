#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cglm/cglm.h>
#include <math.h>
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Sprite.h"
#include "Util.h"

#include <time.h>

#define WINDOW_TITLE "OpenGL SandboxC"

static int width = 1920, height = 1080;

Model *models[6];
Sprite *crosshair;
clock_t last_frame;

unsigned int shaded_cube_vao, shaded_cube_shader, shaded_cube_vertex_count = 36;
unsigned int shaded_cube_u_model, shaded_cube_u_view, shaded_cube_u_projection, shaded_cube_u_view_pos, shaded_cube_u_light_pos, shaded_cube_u_light_color, shaded_cube_u_object_color;

float shaded_cube_verticies[] = {
        -1.f, -1.f,  1.f  ,  0.0f,  0.0f, 1.0f, // 0
         1.f, -1.f,  1.f  ,  0.0f,  0.0f, 1.0f, // 1
         1.f,  1.f,  1.f  ,  0.0f,  0.0f, 1.0f, // 2
         1.f,  1.f,  1.f  ,  0.0f,  0.0f, 1.0f, // 2
        -1.f,  1.f,  1.f  ,  0.0f,  0.0f, 1.0f, // 3
        -1.f, -1.f,  1.f  ,  0.0f,  0.0f, 1.0f, // 0

         1.f, -1.f,  1.f  ,  1.0f,  0.0f,  0.0f, // 1
         1.f, -1.f, -1.f  ,  1.0f,  0.0f,  0.0f, // 5
         1.f,  1.f, -1.f  ,  1.0f,  0.0f,  0.0f, // 6
         1.f,  1.f, -1.f  ,  1.0f,  0.0f,  0.0f, // 6
         1.f,  1.f,  1.f  ,  1.0f,  0.0f,  0.0f, // 2
         1.f, -1.f,  1.f  ,  1.0f,  0.0f,  0.0f, // 1

         1.f,  1.f, -1.f  ,  0.0f,  0.0f, -1.0f, // 6
         1.f, -1.f, -1.f  ,  0.0f,  0.0f, -1.0f, // 5
        -1.f, -1.f, -1.f  ,  0.0f,  0.0f, -1.0f, // 4
        -1.f, -1.f, -1.f  ,  0.0f,  0.0f, -1.0f, // 4
        -1.f,  1.f, -1.f  ,  0.0f,  0.0f, -1.0f, // 7
         1.f,  1.f, -1.f  ,  0.0f,  0.0f, -1.0f, // 6

        -1.f,  1.f, -1.f  , -1.0f,  0.0f,  0.0f, // 7
        -1.f, -1.f, -1.f  , -1.0f,  0.0f,  0.0f, // 4
        -1.f, -1.f,  1.f  , -1.0f,  0.0f,  0.0f, // 0
        -1.f, -1.f,  1.f  , -1.0f,  0.0f,  0.0f, // 0
        -1.f,  1.f,  1.f  , -1.0f,  0.0f,  0.0f, // 3
        -1.f,  1.f, -1.f  , -1.0f,  0.0f,  0.0f, // 7

         1.f, -1.f, -1.f  ,  0.0f, -1.0f,  0.0f, // 5
         1.f, -1.f,  1.f  ,  0.0f, -1.0f,  0.0f, // 1
        -1.f, -1.f,  1.f  ,  0.0f, -1.0f,  0.0f, // 0
        -1.f, -1.f,  1.f  ,  0.0f, -1.0f,  0.0f, // 0
        -1.f, -1.f, -1.f  ,  0.0f, -1.0f,  0.0f, // 4
         1.f, -1.f, -1.f  ,  0.0f, -1.0f,  0.0f, // 5

         1.f,  1.f,  1.f  ,  0.0f,  1.0f,  0.0f, // 2
         1.f,  1.f, -1.f  ,  0.0f,  1.0f,  0.0f, // 6
        -1.f,  1.f, -1.f  ,  0.0f,  1.0f,  0.0f, // 7
        -1.f,  1.f, -1.f  ,  0.0f,  1.0f,  0.0f, // 7
        -1.f,  1.f,  1.f  ,  0.0f,  1.0f,  0.0f, // 3
         1.f,  1.f,  1.f  ,  0.0f,  1.0f,  0.0f, // 2
};

static void setup()
{
    unsigned int texture_shader = compile_shader("res/shader/texture_vertex.glsl", "res/shader/texture_fragment.glsl");
    unsigned int color_shader = compile_shader("res/shader/color_vertex.glsl", "res/shader/color_fragment.glsl");
    shaded_cube_shader = compile_shader("res/shader/lighting_vertex.glsl", "res/shader/lighting_fragment.glsl");

    {
        glGenVertexArrays(1, &shaded_cube_vao);
        glBindVertexArray(shaded_cube_vao);

        unsigned int shaded_cube_vertex_buffer;
        glGenBuffers(1, &shaded_cube_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, shaded_cube_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(shaded_cube_verticies), shaded_cube_verticies, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        shaded_cube_u_model        = glGetUniformLocation(shaded_cube_shader, "model");
        shaded_cube_u_view         = glGetUniformLocation(shaded_cube_shader, "view");
        shaded_cube_u_projection   = glGetUniformLocation(shaded_cube_shader, "projection");
        shaded_cube_u_view_pos     = glGetUniformLocation(shaded_cube_shader, "viewPos");
        shaded_cube_u_light_pos    = glGetUniformLocation(shaded_cube_shader, "lightPos");
        shaded_cube_u_light_color  = glGetUniformLocation(shaded_cube_shader, "lightColor");
        shaded_cube_u_object_color = glGetUniformLocation(shaded_cube_shader, "objectColor");
        
        glBindVertexArray(0);
    }

    Mesh *cube_mesh = create_cube_mesh();
    Texture *default_tex = create_texture("res/texture/128x128.png", GL_LINEAR, GL_NEAREST, GL_CLAMP_TO_EDGE);
    
    int i;
    for (i = 0; i < 6; ++i) {
        models[i] = create_model(cube_mesh);
        models[i]->shader_program = texture_shader;
    }
    models[0]->position[0] = 4.5f;
    models[1]->scale[0] = 2.f;
    models[2]->position[1] = -4.f;
    models[2]->scale[0] = 3.f;
    models[2]->rotation[1] = (float)GLM_PI;
    models[3]->shader_program = color_shader;
    models[3]->position[2] = -5.f;
    models[3]->scale[0] = 2.f;
    models[3]->scale[1] = 2.f;
    models[3]->scale[2] = 2.f;
    models[3]->rotation[2] = (float)GLM_2_PI;
    models[0]->mesh->texture = default_tex;
    models[1]->mesh->texture = default_tex;
    models[2]->mesh->texture = default_tex;
    models[3]->mesh->texture = default_tex;
    models[4] = create_model(create_cylinder_mesh(32));
    models[4]->mesh->texture = default_tex;
    models[4]->shader_program = texture_shader;
    models[4]->position[0] = -4.5f;
    models[4]->rotation[0] = (float)-GLM_PI_2;
    models[4]->rotation[2] = (float)GLM_PI_2;
    models[4]->scale[1] = 3.f;
    models[5]->shader_program = color_shader;
    models[5]->scale[0] = 0.1f;
    models[5]->scale[1] = 0.1f;
    models[5]->scale[2] = 0.1f;

    crosshair = create_sprite(create_texture("res/texture/crosshair.png", GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_BORDER));
    crosshair->position[0] = width / 2.f;
    crosshair->position[1] = height / 2.f;

    last_frame = clock();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

static vec3 camera_position = {0.f, 0.f, 0.f};
static vec3 camera_rotation = {0.f, 0.f, 0.f};
static unsigned char forward = 0, back = 0, left = 0, right = 0 , up = 0, down = 0;
static unsigned char active = 0, pause = 0, focus = 0;

static void handle_movement(float delta)
{
    if(!active) return;
    vec3 direction = {0.f, 0.f, 0.f};
    if(left)
        direction[0] += 1.f;
    if(right)
        direction[0] -= 1.f;
    if(forward)
        direction[2] += 1.f;
    if(back)
        direction[2] -= 1.f;
    if(up)
        direction[1] += 1.f;
    if(down)
        direction[1] -= 1.f;
    glm_vec3_norm(direction);
    glm_vec3_rotate(direction, -1.f * camera_rotation[1], (vec3){0.f, 1.f, 0.f});
    delta *= 7.5f;
    glm_vec3_mul(direction, (vec3){delta, delta, delta}, direction);
    glm_vec3_add(direction, camera_position, camera_position);
}

#ifdef _WIN32
static HWND self;

static void handle_mouse()
{
    if(!self)
        self = FindWindow(NULL, WINDOW_TITLE);

    focus = GetForegroundWindow() == self;
    if(focus && !pause)
    {
        glutWarpPointer(width / 2, height / 2);
        glutSetCursor(GLUT_CURSOR_NONE);
    } else {
        glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
    }
    active = focus && !pause;
}
#endif
#ifdef __linux__
static void handle_mouse()
{
    if(!pause)
    {
        glutWarpPointer(width / 2, height / 2);
        glutSetCursor(GLUT_CURSOR_NONE);
    } else {
        glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
    }
    active = !pause;
}
#endif

static float time_passed = 0;

static void mouse_motion(int x, int y)
{
    if(!active) return;
    camera_rotation[0] += glm_rad(((float)y - height / 2) / 10);
    camera_rotation[0] = fmaxf(fminf(glm_rad(85.f), camera_rotation[0]), glm_rad(-85.f));
    camera_rotation[1] += glm_rad(((float)x - width / 2) / 10);
    camera_rotation[1] = fmodf(camera_rotation[1], (float)GLM_PI * 2);
}

static float shaded_cube_angle = 0.0f;
static void display(void)
{
    clock_t cur_time = clock();
    float delta = (cur_time - last_frame) / (float)(CLOCKS_PER_SEC);
    handle_mouse();
    handle_movement(delta);
    last_frame = cur_time;
    time_passed += (float)delta;

    glClearColor(1.0f, 0.2f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    models[0]->rotation[0] += (float)delta;
    models[0]->rotation[1] += (float)delta;
    models[0]->rotation[2] += (float)delta;
    models[2]->position[0] = sinf(time_passed) * 2;

    mat4 proj = GLM_MAT4_IDENTITY_INIT;
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    glm_rotate(view, camera_rotation[0], (vec3){1.f, 0.f, 0.f});
    glm_rotate(view, camera_rotation[1], (vec3){0.f, 1.f, 0.f});
    glm_translate(view, camera_position);
    glm_perspective(glm_rad(70.f), (float)width / height, 0.01f, 100.f, proj);
    glm_translate_z(view, -5.5f);

    mat4 proj_view;
    glm_mul(proj, view, proj_view);

    {
        models[5]->position[0] = sinf(shaded_cube_angle) * 4.0f;
        models[5]->position[1] = 3.0f;
        models[5]->position[2] = cosf(shaded_cube_angle) * 4.0f + 6.0f;

        shaded_cube_angle += delta;
        shaded_cube_angle = fmodf(shaded_cube_angle, GLM_PI * 2);

        glBindVertexArray(shaded_cube_vao);
        glUseProgram(shaded_cube_shader);

        glUniform3f(shaded_cube_u_object_color, 1.0f, 0.5f, 0.31f);
        glUniform3f(shaded_cube_u_light_color, 1.0f, 1.0f, 1.0f);
        glUniform3f(shaded_cube_u_light_pos, models[5]->position[0], models[5]->position[1], models[5]->position[2]);
        glUniform3fv(shaded_cube_u_view_pos, 1, camera_position);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(model, (vec3){ 0.f, 0.0f, 6.0f });

        glUniformMatrix4fv(shaded_cube_u_model, 1, GL_FALSE, model[0]);
        glUniformMatrix4fv(shaded_cube_u_view, 1, GL_FALSE, view[0]);
        glUniformMatrix4fv(shaded_cube_u_projection, 1, GL_FALSE, proj[0]);

        glDrawArrays(GL_TRIANGLES, 0, shaded_cube_vertex_count);
        glBindVertexArray(0);
    }

    int i;
    for(i = 0; i < 6; ++i)
        draw_model(models[i], proj_view);
    for(i = 0; i < 1; ++i)
        draw_sprite(crosshair);

    glutSwapBuffers();
    glutPostRedisplay();
}

static void keyboard(unsigned char button, int x, int y)
{
    switch(button)
    {
        case 'w':
        case 'W':
            forward = 1;
            break;
        case 'a':
        case 'A':
            left = 1;
            break;
        case 's':
        case 'S':
            back = 1;
            break;
        case 'd':
        case 'D':
            right = 1;
            break;
        case 'q':
        case 'Q':
            up = 1;
            break;
        case 'e':
        case 'E':
            down = 1;
            break;
        case 'p':
        case 'P':
            pause = !pause;
            break;
    }
}

static void keyboard_up(unsigned char button, int x, int y)
{
 switch(button)
    {
        case 'w':
        case 'W':
            forward = 0;
            break;
        case 'a':
        case 'A':
            left = 0;
            break;
        case 's':
        case 'S':
            back = 0;
            break;
        case 'd':
        case 'D':
            right = 0;
            break;
        case 'q':
        case 'Q':
            up = 0;
            break;
        case 'e':
        case 'E':
            down = 0;
            break;
    }
}

static void reshape(int w, int h)
{
    width = w > 1 ? w : 1;
    height = h > 1 ? h : 1;
    glViewport(0, 0, width, height);
    crosshair->position[0] = width / 2.f;
    crosshair->position[1] = height / 2.f;
    glClearDepth(1.0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
}


#ifdef GLAD_DEBUG
void pre_gl_call(const char *name, void *funcptr, int len_args, ...) {
    printf("Calling: %s (%d arguments)\n", name, len_args);
}
#endif

int main(int argc, char **argv)
{
    if(gladLoadGL()) {
        // you need an OpenGL context before loading glad
        printf("I did load GL with no context!\n");
        exit(-1);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow(WINDOW_TITLE);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);
    glutPassiveMotionFunc(mouse_motion);
    glutMotionFunc(mouse_motion);

    if(!gladLoadGL()) {
        printf("Something went wrong!\n");
        exit(-1);
    }

#ifdef GLAD_DEBUG
    // before every opengl call call pre_gl_call
    glad_set_pre_callback(pre_gl_call);
    // don't use the callback for glClear
    // (glClear could be replaced with your own function)
    glad_debug_glClear = glad_glClear;
#endif

    // gladLoadGLLoader(&glutGetProcAddress);
    printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
    if (GLVersion.major < 2) {
        printf("Your system doesn't support OpenGL >= 2!\n");
        return -1;
    }

    printf("OpenGL %s, GLSL %s\n%s\n", glGetString(GL_VERSION),
           glGetString(GL_SHADING_LANGUAGE_VERSION), glGetString(GL_RENDERER));

    setup();

    glutMainLoop();

    return 0;
}
