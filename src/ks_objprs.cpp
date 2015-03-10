#include "ks_objprs.h"

#include <stdio.h>
#include <string.h>

b32
parseObj( const char *filename, Mesh *m)
{
    //TODO(kasper): Buffer overflow
    char buffer[32];

    FILE *file;
    if( fopen_s( &file, filename, "r")) {
        printf("Failed to open file %s\n", filename);
        return 0;
    }

    strncpy( m->name, filename, 32);

    while( fscanf( file, "%s", buffer) != EOF) {
        if( strcmp( buffer, "v" ) == 0) {
            vec3 v;
            fscanf( file, "%f %f %f\n", &v.x, &v.y, &v.z);
            m->vertices.push_back( v);

        } else if( strcmp( buffer, "vn") == 0) {
            vec3 vn;
            fscanf( file, "%f %f %f\n", &vn.x, &vn.y, &vn.z);
            m->vertexNormals.push_back( vn);

        } else if( strcmp( buffer, "f") == 0) {
            u32 i1, i2, i3;
            fscanf( file, "%d//%*d %d//%*d %d//%*d\n",
                    &i1, &i2, &i3);
            m->vertexIndices.push_back(i1);
            m->vertexIndices.push_back(i2);
            m->vertexIndices.push_back(i3);

        }
    }


    fclose( file);
    return 1;
}
