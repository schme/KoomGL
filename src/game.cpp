#include "game.h"
#include "kms_glutils.h"


static MemoryStack *memory;

static GLuint shaderProgram = 0;
static GLuint vao = 0;
static GLuint vertexBufferObject = 0;

real32 screen_vertices[] = 
{
-1.0f, -1.0f, 1.0f,
1.0f, -1.0f, 1.0f,
1.0f, 1.0f, 1.0f,

1.0f, 1.0f, 1.0f,
-1.0f, 1.0f, 1.0f,
-1.0f, -1.0f, 1.0f,
};

void renderInit()
{

    std::vector<GLint> shaders;
    shaders.push_back( loadShader( SHADERPATH("min.vert"), GL_VERTEX_SHADER));
    shaders.push_back( loadShader( SHADERPATH("min.frag"), GL_FRAGMENT_SHADER));

    shaderProgram = createProgram( shaders);
    assert(shaderProgram);

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);

    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData( GL_ARRAY_BUFFER, sizeof( screen_vertices), screen_vertices, GL_STATIC_DRAW);
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

void draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram( shaderProgram);

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
    renderInit();
}


void gameUpdateAndRender( GameInput input)
{
    draw();
}

void resize( int w, int h) {
    glViewport( 0, 0, (GLsizei) w, (GLsizei) h);
}
