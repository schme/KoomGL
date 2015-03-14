#include "game.h"

#ifndef SCENE_H_
#define SCENE_H_

Material scene_materials[] =
{
    { 0.0f, 0.0f, 0.0f, 0.0f},
    { 0.2f, 0.9f, 0.8f, 16.0f},
    { 0.5f, 0.5f, 0.6f, 1.0f},
};

Sphere scene_spheres[] = {
    {   vec3{{-1.0f, -0.5f, 10.0f }},
        vec3{{ 0.7f,  0.7f, 0.7f  }},
        5.0f, 1
    },
    {   vec3{{ 7.0f, -1.0f, 15.0f }},
        vec3{{ 0.3f,  0.0f, 0.0f  }},
        5.0f, 1
    },
};

Plane scene_planes[] = {
    {   vec3{{ 0.3f,  0.3f, 0.5f }},
        vec3{{ 0.0f,  1.0f, 0.0f }},
        7.5f, 2
    },
};

Light scene_lights[] = {
    {   vec3{{ 10.0f,  20.0f,  10.0f }},
        vec3{{ 0.2f,   0.1f,  0.5f  }},
        vec3{{ 0.9f,  0.85f,  1.0f  }},
    },
    {   vec3{{ -5.0f,  30.0f, 5.0f }},
        vec3{{ 0.2f,   0.5f,  0.2f  }},
        vec3{{ 0.9f,  0.85f,  0.9f  }},
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
