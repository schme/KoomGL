#include "game.h"
#include "ks_glutils.h"
#include "ks_objprs.h"

#include <stdio.h>

static MemoryStack *memory;

static real64 globalTime = 0.0f; // in seconds

static GLuint shaderPrograms[10];
static GLuint vao;
static GLuint vertexBufferObject;
// uniforms
static GLuint globalTimeUnif;

static Camera camera;
static Mesh *cube;

real32 background_vertices[] = 
{
-1.0f, -1.0f, 1.0f,
1.0f, -1.0f, 1.0f,
1.0f, 1.0f, 1.0f,

1.0f, 1.0f, 1.0f,
-1.0f, 1.0f, 1.0f,
-1.0f, -1.0f, 1.0f,
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

    shaderPrograms[0] = createProgram(SHADERPATH("min.vert"), SHADERPATH("min.frag"));
    assert(shaderPrograms[0]);

    globalTimeUnif = glGetUniformLocation( shaderPrograms[0], "globalTime");

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);

    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData( GL_ARRAY_BUFFER, sizeof( background_vertices), background_vertices, GL_STATIC_DRAW);
    glBindBuffer( GL_ARRAY_BUFFER, 0);
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnable( GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable( GL_DEPTH_TEST);
    glDepthMask( GL_TRUE);
    glDepthFunc( GL_LEQUAL);
    glDepthRange( 1.0f, 0.0f);
    glClearDepth(1.0f);

    glEnable( GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

}

void draw(real64 globalTime)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram( shaderPrograms[0]);

    glUniform1f( globalTimeUnif, globalTime);
    glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays( GL_TRIANGLES, 0, 8);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}


void gameInit(MemoryStack *ms)
{
    memory = ms;

    camera.pos = glm::vec3( 3, 15, 2);

    cube = (Mesh*)popMemoryStack( memory, sizeof(Mesh));
    parseObj( "assets/cube.obj", cube);

    renderInit();
}


void gameUpdateAndRender( GameInput input)
{
    handleInput( input);
    draw(globalTime);
    globalTime += (input.deltaTime / 1000.0f);
}

void resize( int w, int h) {
    glViewport( 0, 0, (GLsizei) w, (GLsizei) h);
}
