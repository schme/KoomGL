#include "game.h"
#include "scene.h"

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
                        Lerp( 0.0f, 255.0f,
                            Clamp(0.0f, 1.0f, c)));
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


b32
ShadowRayReachedLight( Ray ray )
{
    for (u32 n = 0; n < numSpheres; ++n)
    {
        r32 rad = spheres[n].rad;
        vec3 v = (ray.pos + ray.dir * eps) - spheres[n].pos;
        //vec3 v = ray.pos - spheres[n].pos;
        r32 B = 2.0f*( Dot( ray.dir, v));
        r32 C = LengthSq(v) - Squared(rad);
        r32 D = Squared(B) - 4.0f*C;    // A=1 when ray.dir is normalized

        if( D < 0.0f) continue;

        r32 t = (-B - Sqrt(D))/2.0f;
        if( t < 0.0f) {
            t = (-B + Sqrt(D))/2.0f;
        }

        if( t > 0.0f ) return false;
    }

    for (u32 n = 0; n < numPlanes; ++n)
    {
        r32 vd = Dot(planes[n].normal, ray.dir);
        if( vd == 0.0f ) continue;

        r32 d2 = -(Dot(planes[n].normal, ray.pos) + planes[n].pos);
        r32 t = d2 / vd;
        if( t > eps ) return false;
    }

    return true;
}

Hit
RayObjectsIntersect( Ray *ray, Intersection *inters)
{
    Hit hit = InitHit();

    for (u32 n = 0; n < numSpheres; ++n)
    {
        r32 rad = spheres[n].rad;
        vec3 v = ray->pos - spheres[n].pos;
        //vec3 v = ray.pos - spheres[n].pos;
        r32 B = 2.0f*( Dot( ray->dir, v));
        r32 C = LengthSq(v) - Squared(rad);
        r32 D = Squared(B) - 4.0f*C;    // A=1 when ray.dir is normalized

        if( D < 0.0f) continue;
        r32 t = (-B - Sqrt(D))/2.0f;
        if( t < 0.0f) {
            t = (-B + Sqrt(D))/2.0f;
        }

        if( t < 0.0f || t >= inters->distance ) continue;
        inters->distance = t;
        inters->point = (ray->dir * t) + ray->pos;
        inters->normal = Norm((inters->point - spheres[n].pos) * (1.0f/rad));
        hit.index = n;
        hit.type = HitType::Sphere;
    }

    for (u32 n = 0; n < numPlanes; ++n)
    {
        r32 vd = Dot(planes[n].normal, ray->dir);
        if( vd == 0.0f ) continue;

        r32 d2 = -(Dot(planes[n].normal, ray->pos) + planes[n].pos);
        r32 t = d2 / vd;
        if( t < eps || t >= inters->distance) continue;
        inters->distance = t;
        inters->point = (ray->dir * t) + ray->pos;
        inters->normal = vd > 0 ? -planes[n].normal : planes[n].normal;
        hit.index = n;
        hit.type = HitType::Plane;
    }
    return hit;
}


vec3
DirectLighting(vec3 view, Intersection intersection, Material material )
{
    const vec3 ambient_color = {{0.12f, 0.12f, 0.12f}};
    vec3 color = {};

    vec3 diffuse = {};
    vec3 specular = {};
    vec3 ambient = material.ambient * ambient_color;

    for (u32 i = 0; i < numLights; ++i)
    {
        Intersection sinters = InitIntersection();

        Ray shadow;
        shadow.pos = intersection.point;
        shadow.dir = Norm(lights[i].pos - intersection.point);

        if( !ShadowRayReachedLight( shadow) ) continue;

        /** Direct color using Phong Model **/
        r32 lambertian = Max( Dot(shadow.dir, intersection.normal), 0.0f);
        r32 specAngle = 0.0f;

        if( lambertian > 0.0f) {
            vec3 reflectDir = Reflect( -shadow.dir, intersection.normal);
            specAngle = Max( Dot(reflectDir, view), 0.0f);
        }


        diffuse += material.diffuse * (lambertian * lights[i].diffuseColor ) ;
        specular += material.specular * (pow( specAngle, material.alpha) *
                                                     lights[i].specularColor);
    }

    color = ambient + diffuse + specular;
    return color;
}


void
RayTrace( Ray *ray)
{
    if( ray->rec_depth <= 0) {
        return;
    }

    Intersection inters = InitIntersection();

    vec3 color_direct = {};
    Hit hit = RayObjectsIntersect( ray, &inters);

    if( hit.type != HitType::None) {

        Material material;

        switch (hit.type) {
            case HitType::Plane: {
                material = materials[ planes[hit.index].matIndex];
            } break;
            case HitType::Sphere: {
                material = materials[ spheres[hit.index].matIndex];
            } break;
            default: {
                material = materials[0];
            } break;
        }

        color_direct = DirectLighting( ray->dir, inters, material);
        *ray->pixel += ray->attenuation * color_direct;

        /** transmitted light */
        Ray transmitted;
        transmitted.pixel = ray->pixel;
        transmitted.attenuation = ray->attenuation * transmission_coefficient;
        transmitted.pos = inters.point;
        //transmitted.dir = transmission_dir;
        transmitted.rec_depth = ray->rec_depth - 1;
        //RayTrace( &transmitted );

        /** reflected light */
        Ray reflected;
        reflected.pixel = ray->pixel;
        reflected.attenuation = ray->attenuation * transmission_coefficient;
        reflected.pos = inters.point;
        reflected.dir = Reflect( ray->dir, inters.normal);
        reflected.rec_depth = ray->rec_depth - 1;
        RayTrace( &reflected );
    }
}


static void
TraceFrame()
{
    const r32 xleft = -12;
    const r32 xright = 12;
    const r32 ybottom = -9;
    const r32 ytop = 9;

    vec3 eye_pos = {{0.0f, 0.0f, -10.0f}};

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

            /* position of the image plane in world coordinates. maybe */
            vec3 vp = {{
                xleft + 0.5f * dx + i * dx,
                ytop - 0.5f * dy - j * dy,
                0.0f
            }};

            vec3 *pixel = &((vec3*)draw_buffer)[index];

            Ray ray = {};
            ray.pixel = pixel;
            ray.attenuation = Vec3(1.0f);
            ray.pos = eye_pos;
            ray.dir = Norm(vp - ray.pos);
            ray.rec_depth = max_recursion_depth;

            RayTrace( &ray);
        }
    }

}


void gameInit(MemoryStack *ms)
{
    memory = ms;

    buffer_size = buf_width * buf_height * sizeof(vec3);
    draw_buffer = (vec3*)popMemoryStack( memory, buffer_size);

    defineScene();
    renderInit();
}


void handleInput(GameInput input)
{
    static GameInput state;
    static b32 saved = false;
    changeInputState( &state, input);
    if( state.KEY_S && !saved) {
        printf(" Saving\n");
        bufferToPPM( "out/image5.ppm");
        printf(" Saving done\n");
        saved = true;
    }
}


void gameUpdateAndRender( GameInput input)
{
    static u32 run_count = 0;
    globalTime += (input.deltaTime / 1000.0f);
    handleInput( input);
#if 1
    if( !run_count) {
        printf(" Tracing\n");
        TraceFrame();
        printf(" Tracing done\n");

    }
#else
    TraceFrame();
#endif


    drawBuffer();
    run_count = 1;
}


void resize( int w, int h) {
    glViewport( 0, 0, (GLsizei) w, (GLsizei) h);
}
