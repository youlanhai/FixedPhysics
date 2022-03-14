//////////////////////////////////////////////////////////////////////
/// Desc  DebugRenderer
/// Time  2020/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once

#include <glad/glad.h>
#include "Matrix.hpp"

class DebugRenderer
{
    DebugRenderer() 
    {}
public:

    static void Init();

    static void CheckGLErrors(const char *file, int line);

    static GLint CompileShader(GLenum type, const char *source);
    static GLint LinkProgram(GLint vshader, GLint fshader);
    static bool ValidateProgram(GLint program);

    static void SetAttribute(GLuint program, char const *name, GLint size, GLenum gltype, GLsizei stride, GLvoid *offset);

    static void DrawDebugData();


    static GLuint program;
    static GLuint vao;
    static GLuint vbo;
    static Matrix worldViewProjMatrix;
};

#define GLSL(x) #x

#define CHECK_GL_ERRORS() DebugRenderer::CheckGLErrors(__FILE__, __LINE__)
