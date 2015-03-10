#include "game.h"
#include "ks_glutils.h"
#include "ks_objprs.h"

#include <stdio.h>

static MemoryStack *memory;

static i32 buf_width = 1024;
static i32 buf_height = 768;
static vec3 *draw_buffer;

GLuint buffer_tex;
GLuint tex_sampler;

static r64 globalTime = 0.0f; // in seconds

static GLuint shaderPrograms[10];
static GLuint vao;
static GLuint vertexBufferObject;
static GLuint indexBufferObject;

// uniforms
static GLuint globalTimeUnif;

static Camera camera;
static Mesh *scene;

r32 background_vertices[] =
{
    // vertices             //texcoords
    -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,
     1.0f, -1.0f, 1.0f,     1.0f, 0.0f,
     1.0f,  1.0f, 1.0f,     1.0f, 1.0f,
    -1.0f,  1.0f, 1.0f,     0.0f, 1.0f,
};

u32 indices[] =
{
    0,1,2,
    2,3,0,
};


void changeInputState( GameInput *state, GameInput input) {

   if( input.KEY_W ) {
        (*state).KEY_W = !(*state).KEY_W;
    }
    if( input.KEY_S) {
        (*state).KEY_S = !(*state).KEY_S;
    }
    if( input.KEY_A) {
        (*state).KEY_A = !(*state).KEY_A;
    }
    if( input.KEY_D) {
        (*state).KEY_D = !(*state).KEY_D;
    }
    if( input.KEY_ESC) {
        (*state).KEY_ESC = !(*state).KEY_ESC;
    }
    if( input.KEY_SPACE) {
        (*state).KEY_SPACE = !(*state).KEY_SPACE;
    }

    if( input.MouseButtons[0]) {
        (*state).MouseButtons[0] = !(*state).MouseButtons[0];
    }
    if( input.MouseButtons[1]) {
        (*state).MouseButtons[1] = !(*state).MouseButtons[1];
    }
    if( input.MouseButtons[2]) {
        (*state).MouseButtons[2] = !(*state).MouseButtons[2];
    }
    if( input.MouseButtons[3]) {
        (*state).MouseButtons[3] = !(*state).MouseButtons[3];
    }
    if( input.MouseButtons[4]) {
        (*state).MouseButtons[4] = !(*state).MouseButtons[4];
    }
}


void handleInput(GameInput input)
{
    static GameInput state;
    changeInputState( &state, input);
}

void renderInit()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    shaderPrograms[0] = createProgram(SHADERPATH("min.vert"), SHADERPATH("min.frag"));
    assert(shaderPrograms[0]);

    globalTimeUnif = glGetUniformLocation( shaderPrograms[0], "globalTime");
    tex_sampler = glGetUniformLocation( shaderPrograms[0], "sampler");

    glClearColor( 0.1f, 0.0f, 0.0f, 1.0f);

    glGenBuffers( 1, &vertexBufferObject);
    glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData( GL_ARRAY_BUFFER, sizeof( background_vertices), background_vertices, GL_STATIC_DRAW);

    glGenBuffers( 1, &indexBufferObject);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(r32), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(r32), (void*)(3 * sizeof(r32)));


    glActiveTexture( GL_TEXTURE0);
    glGenTextures(1, &buffer_tex);
    glBindBuffer( GL_TEXTURE_2D, buffer_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram( shaderPrograms[0]);
    glUniform1i( tex_sampler, 0);


    glUseProgram(0);

    //glEnable( GL_BLEND);
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable( GL_DEPTH_TEST);
    //glDepthMask( GL_TRUE);
    //glDepthFunc( GL_LEQUAL);
    //glDepthRange( 1.0f, 0.0f);
    //glClearDepth(1.0f);

    glEnable( GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

}

void fillBuffer()
{
    u32 index = 0;
    for (int i = 0; i < buf_height; ++i)
    {
        for (int j = 0; j < buf_width; ++j)
        {

            index = i * buf_width + j;
            vec3 *pixel = &((vec3*)draw_buffer)[index];

            pixel->r = sinf( (r32)i);
            pixel->g = sinf( (r32)j);
            pixel->b = sinf( globalTime);
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, buf_width, buf_height, 0, GL_RGB, GL_FLOAT, draw_buffer);
}


void drawPixels()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glUseProgram( shaderPrograms[0]);

    glUniform1f( globalTimeUnif, globalTime);

    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUseProgram(0);

}

void gameInit(MemoryStack *ms)
{
    memory = ms;
    camera.pos = vec3{ 3.0f, 15.0f, 2.0f};

    u32 draw_buffer_size = buf_width * buf_height * sizeof(vec3);
    draw_buffer = (vec3*)popMemoryStack( memory, draw_buffer_size);

    scene = (Mesh*)popMemoryStack( memory, sizeof(Mesh));
    parseObj( "assets/scene.obj", scene);

    renderInit();
}


void gameUpdateAndRender( GameInput input)
{
    globalTime += (input.deltaTime / 1000.0f);
    handleInput( input);
    fillBuffer();
    drawPixels();
}

void resize( int w, int h) {
    glViewport( 0, 0, (GLsizei) w, (GLsizei) h);
}


void draw(r64 globalTime)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram( shaderPrograms[0]);

    glUniform1f( globalTimeUnif, globalTime);
    glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject);

    glDrawArrays( GL_TRIANGLES, 0, 8);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}
