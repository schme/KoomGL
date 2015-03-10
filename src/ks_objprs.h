#ifndef KS_OBJPRS_H_
#define KS_OBJPRS_H_

#include "ks_math.h"
#include <glm/vec3.hpp>
#include <vector>

struct Mesh {
    std::vector<vec3> vertices;
    std::vector<vec3> vertexNormals;
    std::vector<u32> vertexIndices;
    char name[32];
};


b32 parseObj( const char *filename, Mesh *g);
#endif // KS_OBJPRS_H_    
