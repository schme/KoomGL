#include "game.h"
#include "ks_glutils.h"
#include "ks_objprs.h"

#include <stdio.h>
/** For handling PPM files */
#include <stdlib.h>

static MemoryStack *memory;

static vec3 *draw_buffer;
static u32 buffer_size;

GLuint buffer_tex;
GLuint tex_sampler;

static r64 globalTime = 0.0f; // in seconds

static GLuint shaderPrograms[10];
static GLuint vao;
static GLuint vertexBufferObject;
static GLuint indexBufferObject;

// uniforms
static GLuint globalTimeUnif;

static Mesh *scene;

r32 background_vertices[] =
{
    // vertices             //texcoords
    -1.0f, -1.0f, 1.0f,     0.0f, 1.0f,
     1.0f, -1.0f, 1.0f,     1.0f, 1.0f,
     1.0f,  1.0f, 1.0f,     1.0f, 0.0f,
    -1.0f,  1.0f, 1.0f,     0.0f, 0.0f,
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

inline
void drawBuffer()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glUseProgram( shaderPrograms[0]);

    glUniform1f( globalTimeUnif, globalTime);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, buf_width, buf_height, 0, GL_RGB, GL_FLOAT, draw_buffer);
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUseProgram(0);

}


static inline char
normColorTo256( r32 c) {
    u32 result = RoundR32ToU32(
                        Lerp( 0.0f, 255.0f, c));
    return (char)result;
}


static void
bufferToPPM(const char *filename)
{
    /** Reserve a hefty amount of memory */
    FILE *fp = fopen(filename, "wb");
    fprintf( fp, "P6\n%d %d\n255\n", buf_width, buf_height);
    for (int j = 0; j < buf_height; ++j)
    {
        for (int i = 0; i < buf_width; ++i)
        {
            vec3 pixel = draw_buffer[j * buf_width + i];
            static char color[3];
            color[0] = normColorTo256( pixel.r);
            color[1] = normColorTo256( pixel.g);
            color[2] = normColorTo256( pixel.b);
            fwrite( color, 1, 3, fp);
        }
    }
    fclose(fp);
}


void
RayObjects( Ray ray, Intersection *inters)
{

    for (u32 n = 0; n < numSpheres; ++n)
    {
        r32 rad = spheres[n].rad;
        vec3 v = (ray.pos + ray.v * (ray.length * eps)) - spheres[n].pos;
        //vec3 v = ray.pos - spheres[n].pos;
        r32 B = 2.0f*( Dot( ray.v, v));
        r32 C = LengthSq(v) - Squared(rad);
        r32 D = Squared(B) - 4.0f*C;    // A=1 when ray.v is normalized

        if( D < 0.0f) continue;
        r32 t = (-B - Sqrt(D))/2.0f;
        if( t < 0.0f) {
            t = (-B + Sqrt(D))/2.0f;
        }

        if( t < 0.0f || t >= inters->distance ) continue;
        inters->index = n;
        inters->distance = t;
        inters->type = HitType::Sphere;
        inters->location = (ray.v * t) + ray.pos;
        inters->normal = (inters->location - spheres[n].pos) * (1.0f/rad);
    }

    for (u32 n = 0; n < numPlanes; ++n)
    {
        r32 vd = Dot(planes[n].normal, ray.v);
        if( vd == 0.0f ) continue;

        r32 d2 = -(Dot(planes[n].normal, ray.pos) + planes[n].pos);
        r32 t = d2 / vd;
        if( t < eps || t >= inters->distance) continue;
        inters->index = n;
        inters->distance = t;
        inters->type = HitType::Plane;
        inters->location = (ray.v * t) + ray.pos;
        inters->normal = vd > 0 ? -planes[n].normal : planes[n].normal;
    }
}


void
RayTrace( vec3 *resultcolor, Ray ray, i32 rec_depth)
{
    Intersection inters = InitIntrs();

    RayObjects( ray, &inters);

    vec3 color = {};

    if( inters.type != HitType::None) {

        for (u32 i = 0; i < numLights; ++i)
        {
            Intersection sinters = InitIntrs();
            Ray sray;
            sray.pos = inters.location;
            sray.v = Norm(lights[i].pos - inters.location);
            sray.length = inters.distance;

            RayObjects( sray, &sinters);

            if( sinters.type == HitType::None) {
                color = (lights[i].color * 0.5f);
            }
        }

        switch (inters.type) {

            case HitType::Plane: {
                *resultcolor = color + (planes[inters.index].color * 0.5f);
            } break;

            case HitType::Sphere: {
                *resultcolor = color + (spheres[inters.index].color * 0.5f);
            } break;

            default: {
            } break;
        }

    }
}


static void
TraceFrame()
{
    const r32 xleft = -12;
    const r32 xright = 12;
    const r32 ybottom = -9;
    const r32 ytop = 9;

    vec3 camera_pos = {{0.0f, 0.0f, -10.0f}};

    /** dx, dy per pixel */
    r32 dx = 24.0f/(r32)buf_width;
    r32 dy = 18.0f/(r32)buf_height;

    u32 index = 0;
    for (int j = 0; j < buf_height; ++j)
    {
        for (int i = 0; i < buf_width; ++i)
        {

            vec3 color = {{0.0f, 0.0f, 0.0f}};

            index = j * buf_width + i;
            vec3 *pixel = &((vec3*)draw_buffer)[index];

            /* position of the image plane in world coordinates. maybe */
            vec3 vp = {{
                xleft + 0.5f * dx + i * dx,
                ytop - 0.5f * dy - j * dy,
                0.0f
            }};

            Ray ray = {};
            ray.pos = camera_pos;
            ray.v = Norm(vp - ray.pos);

            RayTrace( &color, ray, max_depth);
            *pixel = color;

        }
    }

}


void gameInit(MemoryStack *ms)
{
    memory = ms;

    buffer_size = buf_width * buf_height * sizeof(vec3);
    draw_buffer = (vec3*)popMemoryStack( memory, buffer_size);

    planes = (Plane*)popMemoryStack( memory, numPlanes * sizeof(Plane));
    spheres = (Sphere*)popMemoryStack( memory, numSpheres * sizeof(Sphere));
    lights = (Light*)popMemoryStack( memory, numLights * sizeof(Light));

    spheres[0] = Sphere {
        vec3{{-1.0f, -0.5f, 10.0f }},
        vec3{{ 0.7f,  0.7f, 0.7f  }},
        5.0f
    };

    spheres[1] = Sphere {
        vec3{{ 7.0f, -1.0f, 15.0f }},
        vec3{{ 0.3f,  0.0f, 0.0f  }},
        5.0f
    };

    planes[0] = Plane {
        vec3{{ 0.3f,  0.3f, 0.5f }},
        vec3{{ 0.0f,  1.0f, 0.0f }},
        7.5f
    };

    lights[0] = Light {
        vec3{{ 5.0f,  20.0f,  10.0f }},
        vec3{{ 0.9f,  0.85f,  1.0f  }}
    };

    renderInit();
}


void gameUpdateAndRender( GameInput input)
{
    static u32 run_count = 0;
    static b32 saved = false;
    globalTime += (input.deltaTime / 1000.0f);
    handleInput( input);
    if( !run_count) {
        printf(" Tracing\n");
        TraceFrame();
        printf(" Tracing done\n");
    } else if( !saved) {
        printf(" Saving\n");
        bufferToPPM( "out/image4.ppm");
        printf(" Saving done\n");
        saved = true;
    }

    drawBuffer();
    run_count = 1;
}


void resize( int w, int h) {
    glViewport( 0, 0, (GLsizei) w, (GLsizei) h);
}
