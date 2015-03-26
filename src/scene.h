#ifndef SCENE_H_
#define SCENE_H_

#include "render.h"
/*
 * TODO(kasper): Move into a separate file which is read at runtime
 */

vec3 ambient_color = {{0.22f, 0.22f, 0.22f}};

/**
 * material:
 * vec3 diffuse
 * vec3 specular
 * r32 shininess (power)
 * r32 refractive_index
 */
Material scene_materials[] =
{
    {   Vec3( 0.3f, 0.3f, 0.7f),
        Vec3( 0.0f, 0.0f, 0.0f),
        16.0f, 1.4f
    },
    {   Vec3( 0.3f, 0.7f, 0.3f ),
        Vec3( 1.0f, 1.0f, 1.0f ),
        16.0f, 1.4f
    },
    {   Vec3( 0.1f, 0.1f, 0.5f ),
        Vec3( 1.0f, 1.0f, 1.0f ),
        4.0f, 1.4f
    },
    {   Vec3( 0.70f, 0.50f, 0.10f ),
        Vec3( 0.10f, 0.10f, 0.10f ),
        50.0f, 1.4f

    },  // mirror
    {   Vec3( 0.0f, 0.0f, 0.0f ),
        Vec3( 1.0f, 1.0f, 1.0f ),
        100.0f, 1.4f

    },  // transparent
    {   Vec3( 0.10f, 0.10f, 0.1f ),
        Vec3( 0.10f, 0.10f, 0.10f ),
        64.0f, 1.19f
    },
};

/**
 * sphere:
 * vec3 pos;
 * r32 rad;
 * i32 material;
 */
Sphere scene_spheres[] = {
    {   {{  -10.0f, -0.5f, 12.0f }},
        5.0f, 4
    },
    {   {{  3.5f, -0.5f, 6.0f }},
        5.0f, 5
    },
    {   {{  0.0f, 5.0f, 20.0f }},
        8.0f, 3
    },
};

/**
 * plane:
 * vec3 normal
 * r32 position
 * i32 material
 */
Plane scene_planes[] = {
    {
        {{ 0.0f,  1.0f, 0.0f }},
        7.5f, 0
    },
};

/**
 * light:
 * vec3 position
 * vec3 diffuseColor
 * vec3 specularColor
 */
Light scene_lights[] = {
    {   {{ 10.0f,  20.0f,  10.0f }},
        {{ 0.98f,  0.95f,  0.88f  }},
        {{ 0.98f,  0.95f,  0.88f  }},
    },
    {   {{ -5.0f,  30.0f,   5.0f }},
        {{ 0.95f,  0.98f,  0.88f  }},
        {{ 0.95f,  0.98f,  0.88f  }},
    },
};


void
defineScene() {
    numMaterials = ArrCount( scene_materials);
    numPlanes = ArrCount( scene_planes);
    numSpheres = ArrCount( scene_spheres);
    numLights = ArrCount( scene_lights);

    materials = scene_materials;
    planes = scene_planes;
    spheres = scene_spheres;
    lights = scene_lights;

    scene_ambient_color = ambient_color;
}

#endif // SCENE_H_
