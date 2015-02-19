#ifndef KS_OBJPRS_H_
#define KS_OBJPRS_H_

#include "types.h"
#include <glm/vec3.hpp>
#include <vector>

struct Mesh {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> vertexNormals;
    std::vector<uint32> faceVertexIndices;
    char *name;
};


bool32 parseObj( const char *filename, Mesh *g);
#endif // KS_OBJPRS_H_    
