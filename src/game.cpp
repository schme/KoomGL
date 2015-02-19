#include "game.h"
#include "ks_glutils.h"
#include "ks_objprs.h"

static MemoryStack *memory;

static real64 globalTime = 0.0f; // in seconds

static GLuint shaderPrograms[10];
static GLuint vao;
static GLuint vertexBufferObject;
// uniforms
static GLuint globalTimeUnif;

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

void handleInput(GameInput input)
{

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

    cube = (Mesh*)popMemoryStack( memory, sizeof(Mesh));
    parseObj( "assets/cube.obj", cube);

    renderInit();
}


void gameUpdateAndRender( GameInput input)
{
    draw(globalTime);
    globalTime += (input.deltaTime / 1000.0f);
}

void resize( int w, int h) {
    glViewport( 0, 0, (GLsizei) w, (GLsizei) h);
}
