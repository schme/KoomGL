#ifndef SCENE_H_
#define SCENE_H_

#include "game.h"

/**
 * material:
 * vec3 diffuse
 * vec3 ambient
 * vec3 specular
 * r32 alpha
 */
Material scene_materials[] =
{
    {   Vec3( 0.5f, 0.5f, 0.5f),
        Vec3( 0.5f, 0.5f, 0.5f),
        Vec3( 0.6f, 0.6f, 0.6f),
        32.0f
    },
    {   Vec3( 0.8f, 0.2f, 0.2f ),
        Vec3( 0.8f, 0.2f, 0.2f ),
        Vec3( 1.0f, 0.89f, 0.9f ),
        16.0f
    },
    {   Vec3( 0.2f, 0.2f, 0.8f ),
        Vec3( 0.2f, 0.2f, 0.8f ),
        Vec3( 1.0f, 0.98f, 0.9f ),
        16.0f
    },
    {   Vec3( 0.2f, 0.8f, 0.2f ),
        Vec3( 0.2f, 0.8f, 0.2f ),
        Vec3( 1.0f, 0.98f, 0.9f ),
        16.0f
    },  // mirror
    {   Vec3( 0.0f, 0.0f, 0.0f ),
        Vec3( 0.0f, 0.0f, 0.0f ),
        Vec3( 1.0f, 0.98f, 0.9f ),
         32.0f
    },
};

/**
 * sphere:
 * vec3 pos;
 * r32 rad;
 * i32 material;
 */
Sphere scene_spheres[] = {
    {   {{  -7.0f, -0.5f, 12.0f }},
        5.0f, 4
    },
    {   {{  7.0f, -1.0f, 12.0f }},
        5.0f, 4
    },
    {   {{  0.0f, 10.0f, 15.0f }},
        5.0f, 4
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
        {{ 0.88f,  0.85f,  0.88f  }},
        {{ 0.98f,  0.95f,  0.88f  }},
    },
    {   {{ -5.0f,  30.0f,   5.0f }},
        {{ 0.85f,  0.88f,  0.88f  }},
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
}

#endif // SCENE_H_
