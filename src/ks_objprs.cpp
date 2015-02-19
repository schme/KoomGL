#include "ks_objprs.h"

#include <stdio.h>
#include <string.h>

bool32
parseObj( const char *filename, Geometry *g)
{
    //TODO(kasper): Buffer overflow
    char buffer[32];

    FILE *file;
    if( fopen_s( &file, filename, "r")) {
        printf("Failed to open file %s\n", filename);
        return 0;
    }

    while( fscanf( file, "%s", buffer) != EOF) {
        if( strcmp( buffer, "v" ) == 0) {
            glm::vec3 v;
            fscanf( file, "%f %f %f\n", &v.x, &v.y, &v.z);
            g->vertices.push_back( v);

        } else if( strcmp( buffer, "vn") == 0) {
            glm::vec3 vn;
            fscanf( file, "%f %f %f\n", &vn.x, &vn.y, &vn.z);
            g->vertexNormals.push_back( vn);

        } else if( strcmp( buffer, "f") == 0) {
            uint32 i1, i2, i3;
            fscanf( file, "%d//%*d %d//%*d %d//%*d\n", 
                    &i1, &i2, &i3);
            g->faceVertexIndices.push_back(i1);
            g->faceVertexIndices.push_back(i2);
            g->faceVertexIndices.push_back(i3);
        }
    }


    fclose( file);
    return 1;
}
