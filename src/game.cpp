#include "game.h"
#include "scene.h"

#include "ks_glutils.h"

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

    glEnable( GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

}

inline
void drawBuffer(b32 refresh_texture)
{
    glClear( GL_COLOR_BUFFER_BIT );
    glUseProgram( shaderPrograms[0]);

    glUniform1f( globalTimeUnif, globalTime);
    if( refresh_texture) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, buf_width, buf_height, 0, GL_RGB, GL_FLOAT, draw_buffer);
    }
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

/**
 * Using * for dot product:
 *
 * T = (nr(N * I) - (1 - nr^2( 1- (N*I)^2))^1/2 )N - nrI
 */
vec3
CalculateTransmittedRay( vec3 incoming, vec3 normal, r32 eta_i, r32 eta_t)
{
    vec3 T;
    r32 eta_r = eta_i / eta_t;
    r32 angle = eta_r * Dot( normal, incoming);
    angle -= Sqrt( 1-Squared(eta_r) * (1-Squared(Dot(normal, incoming))));
    T = Norm(angle * normal - eta_r * incoming);
    return T;
}

vec3
CalculateTransmittedRay( vec3 incoming, vec3 normal, r32 eta_i, r32 eta_t, r32 sq)
{
    vec3 T;
    r32 eta_r = eta_i / eta_t;
    r32 angle = eta_r * Dot( normal, incoming) - Sqrt(sq);
    T = Norm(angle * normal - eta_r * incoming);
    return T;
}

r32
TotalInternalReflection( vec3 incoming, vec3 normal, r32 eta_i, r32 eta_t)
{
    r32 result;
    r32 eta_r = eta_i / eta_t;
    result = 1-Squared(eta_r) * (1-Squared(Dot(normal, incoming)));
    return result;
}


b32
ShadowRayReachedLight( Ray *ray )
{
    for (u32 n = 0; n < numSpheres; ++n)
    {
        r32 rad = spheres[n].rad;
        vec3 v = (ray->pos + ray->dir * (10.0f*eps)) - spheres[n].pos;
        //vec3 v = ray->pos - spheres[n].pos;
        r32 B = 2.0f*( Dot( ray->dir, v));
        r32 C = LengthSq(v) - Squared(rad);
        r32 D = Squared(B) - 4.0f*C;    // A=1 when ray->dir is normalized

        if( D < 0.0f) continue;

        r32 t = (-B - Sqrt(D))/2.0f;
        if( t < 0.0f) {
            t = (-B + Sqrt(D))/2.0f;
        }

        if( t > 0.0f && t < ray->length) return false;
    }

    for (u32 n = 0; n < numPlanes; ++n)
    {
        r32 vd = Dot(planes[n].normal, ray->dir);
        if( vd >= 0.0f ) continue;

        r32 d2 = -(Dot(planes[n].normal, ray->pos) + planes[n].pos);
        r32 t = d2 / vd;
        if( t > eps && t < ray->length) return false;
    }

    return true;
}

/**
 * TODO(kasper): use a better way to solve the quadratic equation for spheres:
 * https://www.siggraph.org/education/materials/HyperGraph/math/math1.htm
 */
Hit
RayObjectsIntersect( Ray *ray, Intersection *inters)
{
    Hit hit = InitHit();

    for (u32 n = 0; n < numSpheres; ++n)
    {
        r32 rad = spheres[n].rad;
        vec3 v = ray->pos - spheres[n].pos;
        //vec3 v = ray->pos - spheres[n].pos;
        r32 B = 2.0f*( Dot( ray->dir, v));
        r32 C = LengthSq(v) - Squared(rad);
        r32 D = Squared(B) - 4.0f*C;    // A=1 when ray->dir is normalized

        if( D < 0.0f) continue;
        r32 t = (-B - Sqrt(D))/2.0f;
        if( t < 0.0f) {
            t = (-B + Sqrt(D))/2.0f;
        }

        if( t < eps || t >= inters->distance ) continue;
        inters->distance = t;
        inters->point = (ray->dir * t) + ray->pos;
        inters->normal = Norm((inters->point - spheres[n].pos) * (1.0f/rad));
        hit.index = n;
        hit.type = HitType::Sphere;
    }

    for (u32 n = 0; n < numPlanes; ++n)
    {
        r32 vd = Dot(planes[n].normal, ray->dir);
        /**
         * If vd > 0 the normal is pointing away from us. If we remove this culling
         * the intersection normal must be checked
         */
        if( vd >= 0.0f ) continue;

        r32 d2 = -(Dot(planes[n].normal, ray->pos) + planes[n].pos);
        r32 t = d2 / vd;
        if( t < eps || t >= inters->distance) continue;
        inters->distance = t;
        inters->point = (ray->dir * t) + ray->pos;
        inters->normal = planes[n].normal;
        hit.index = n;
        hit.type = HitType::Plane;
    }
    return hit;
}


/**
 * From Wikipedia - Phong reflection model:
 * "each term should only be included if the term's dot product is positive.
 * (Additionally, the specular term should only be included if the dot product
 * of the diffuse term is positive.)"
 */
vec3
DirectLighting(vec3 view, Intersection intersection, Material *material )
{
    vec3 diffuse = Vec3(0.0f);
    vec3 specular = Vec3(0.0f);

    for (u32 i = 0; i < numLights; ++i)
    {

        Ray light;
        light.pos = intersection.point;
        light.dir = lights[i].pos - intersection.point;
        light.length = Length(light.dir);
        light.dir = Norm(light.dir);

        if( !ShadowRayReachedLight( &light) ) continue;

        r32 lambertian = Dot( light.dir, intersection.normal);
        vec3 reflectDir = Reflect( light.dir, intersection.normal);
        r32 specAngle = Dot(reflectDir, view);

        diffuse += lambertian * lights[i].diffuseColor;
        if( specAngle > 0.0f) {
            specular += Pow( specAngle, material->shininess) * lights[i].specularColor;
        }
    }

    vec3 color = ambient_color * material->diffuse +
            diffuse * material->diffuse +
            specular * material->specular;
    return color;
}


void
RayTrace( Ray *ray)
{
    if( ray->rec_depth <= 0 ||
        Length(ray->attenuation) < ray_attenuation_clip)
    {
        return;
    }

    Intersection inters = InitIntersection();

    Hit hit = RayObjectsIntersect( ray, &inters);

    if( hit.type != HitType::None) {

        Material *material;

        switch (hit.type) {
            case HitType::Plane: {

                material = &materials[ planes[hit.index].matIndex];
            } break;
            case HitType::Sphere: {
                material = &materials[ spheres[hit.index].matIndex];
            } break;
            default: {
                material = &materials[0];
            } break;
        }

        vec3 color_direct = DirectLighting( -ray->dir, inters, material);
        *ray->pixel += ray->attenuation * color_direct;


        Ray reflected;
        reflected.pixel = ray->pixel;
        reflected.refractive_index = ray->refractive_index;
        reflected.attenuation = ray->attenuation * material->specular;
        reflected.dir = Reflect( -ray->dir, inters.normal);
        reflected.pos = inters.point + (reflected.dir * inters.distance * eps);
        reflected.rec_depth = ray->rec_depth - 1;
        RayTrace( &reflected );


        Ray transmitted;
        transmitted.attenuation = ray->attenuation * (Vec3(1.0f) - material->specular);

        /** Not always needed, so probably more efficient to check here than after call */
        if( Length(transmitted.attenuation) > ray_attenuation_clip) {
            // into an object
            if( ray->refractive_index == 1.0f) {
                transmitted.dir = CalculateTransmittedRay( -ray->dir, inters.normal,
                                                    1.0f, material->refractive_index);
                transmitted.refractive_index = material->refractive_index;
            // out of an object
            } else {
                r32 sq = TotalInternalReflection( -ray->dir, inters.normal,
                                                    material->refractive_index, 1.0f);
                // no total internal reflection, carry on with inversed normal
                if( sq > 0.0f) {
                    transmitted.dir = CalculateTransmittedRay( -ray->dir, -inters.normal,
                                                        material->refractive_index, 1.0f, sq);
                    transmitted.refractive_index = 1.0f;
                // total internal reflection, cull the ray
                } else {
                    //TODO(kasper): structure this section better so this dirty hack isn't needed
                    transmitted.attenuation = Vec3(0.0f);
                    transmitted.dir = Vec3(0.0f);
                }
            }
            transmitted.pixel = ray->pixel;
            transmitted.pos = inters.point + (transmitted.dir * inters.distance * eps);
            transmitted.rec_depth = ray->rec_depth - 1;
            RayTrace( &transmitted );
        }
    }
}


static void
TraceFrame()
{
    const r32 xleft = -8;
    const r32 xright = 8;
    const r32 ybottom = -4.5;
    const r32 ytop = 4.5;

    vec3 eye_pos = {{0.0f, 0.0f, -10.0f}};

    /** dx, dy per pixel */
    r32 dx = (xright*2.0f)/(r32)buf_width;
    r32 dy = (ytop*2.0f)/(r32)buf_height;

    const i32 sample_count = 16;
    i32 aa_grid_side = (i32)Sqrt((r32)sample_count);
    vec2 aa_grid[sample_count];

    r32 sdx = dx/(sample_count / 2.0f);
    r32 sdy = dy/(sample_count / 2.0f);


    for (int j = 0; j < aa_grid_side; ++j)
    {
        for (int i = 0; i < aa_grid_side; ++i)
        {
            u32 index = aa_grid_side * j + i;
            aa_grid[index] = Vec2( sdx + sdx * i, sdy + sdy * j);
        }
    }

    u32 index = 0;
    for (int j = 0; j < buf_height; ++j)
    {
        for (int i = 0; i < buf_width; ++i)
        {
            index = j * buf_width + i;
            vec3 *pixel = &((vec3*)draw_buffer)[index];

            for (int sample = 0; sample < sample_count; ++sample)
            {

                vec3 vp = {{
                    xleft + aa_grid[sample].x + i * dx,
                    ytop - aa_grid[sample].y - j * dy,
                    -4.0f
                }};

                Ray ray = {};
                ray.pixel = pixel;
                ray.attenuation = Vec3(1.0f);
                ray.pos = eye_pos;
                ray.dir = vp - ray.pos;
                ray.length = Length(ray.dir);
                ray.dir = Norm(ray.dir);
                ray.rec_depth = max_recursion_depth;
                ray.refractive_index = 1.0f;    // air

                RayTrace( &ray);
            }
            (*pixel) = (*pixel) / (r32)sample_count;
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
        bufferToPPM( "out/image.ppm");
        printf(" Saving done\n");
        saved = true;
    }
}


void gameUpdateAndRender( GameInput input)
{
    globalTime += (input.deltaTime / 1000.0f);
    handleInput( input);
    b32 refresh_texture = true;
#if 1
    r32 trace_frame_start = 0.0f;
    if( input.frame == 0) {
        trace_frame_start = globalTime;
        printf(" Tracing\n");
        TraceFrame();
        printf(" Tracing done\n");

    } else if ( input.frame == 1) {
        printf(" ~trace frame time: %.2fs\n", globalTime - trace_frame_start);
    }
#else
    TraceFrame();
#endif

    drawBuffer(refresh_texture);
    refresh_texture = false;
}


void resize( int w, int h) {
    glViewport( 0, 0, (GLsizei) w, (GLsizei) h);
}
