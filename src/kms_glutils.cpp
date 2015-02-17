#include "kms_glutils.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>


std::string fileToString( const char *filename) {
    std::ifstream file( filename);
    if( !file.is_open()) {
        assert( 0 && "FAIL: Opening file ");
        std::string msg("FAIL: Opening file: ");
        msg += filename; msg += "\n";
        std::cout << msg.c_str() << std::endl;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

GLint loadShader( const char *filename, GLenum shaderType) {

    std::string shader = fileToString( filename);

    GLuint shaderObject = glCreateShader( shaderType);
    glShaderSource( shaderObject, 1, (const GLchar**)&shader, NULL);
    glCompileShader( shaderObject);
    
    GLint status;
    glGetShaderiv( shaderObject, GL_COMPILE_STATUS, &status);
    if( status == GL_FALSE) {

#if ENABLE_CONSOLE
        PRINT_SHADER_ERROR( shaderObject);
#endif
        assert(!"Shader didn't compile!");
    } else {
#if ENABLE_CONSOLE
        printf("Shader compilation succeeded\n");
#endif
    }

    return shaderObject;
}

GLint createProgram( std::vector<GLint> &shaders) {

    GLint shaderProgram = glCreateProgram();
    for (int i = 0; i < shaders.size(); ++i)
    {
        glAttachShader( shaderProgram, shaders[i]);
    }
    glLinkProgram( shaderProgram);
    for (int i = 0; i < shaders.size(); ++i)
    {
        glDetachShader( shaderProgram, shaders[i]);
    }

    GLint status;
    glGetProgramiv( shaderProgram, GL_LINK_STATUS, &status);
    if( status == GL_FALSE) {
        assert(0 && "Shader linking failed!\n");
        std::string msg( "Program linking failure: " );

        GLint infoLogLength;
        glGetProgramiv( shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength );
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog( shaderProgram, infoLogLength, NULL, strInfoLog );
        msg += strInfoLog;
        delete[] strInfoLog;
        std::cout << msg.c_str() << std::endl;

        glDeleteProgram( shaderProgram ); shaderProgram = 0;
    }
    return shaderProgram;
}
