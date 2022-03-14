//////////////////////////////////////////////////////////////////////
/// Desc  DebugRenderer
/// Time  2020/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "DebugRenderer.hpp"
#include "debug/LogTool.hpp"
#include "debug/DebugDraw.hpp"


#include <cstdlib>
#include <cassert>

NS_FXP_USING;

GLuint DebugRenderer::program = 0;
GLuint DebugRenderer::vao = 0;
GLuint DebugRenderer::vbo = 0;
Matrix DebugRenderer::worldViewProjMatrix = Matrix::Identity;

const char *vertexShader = ""
"#version 330 core \n"
"in vec4 a_position; \n"
"in vec4 a_color; \n"
"uniform mat4 u_matWorldViewProj; \n"
"out vec4 v_color; \n"
"void main()\n"
"{"
"    gl_Position = u_matWorldViewProj * a_position; \n"
"    v_color = a_color; \n"
"}";

const char* fragmentShader = ""
"#version 330 core\n"
"out vec4 FragColor; \n"
"in vec4 v_color; \n"
"void main()\n"
"{"
"    FragColor = v_color; \n"
"}";

#define cpAssertHard(exp, msg) if(!exp){ assert(false && msg); }

void DebugRenderer::CheckGLErrors(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        LOG_ERROR("GLError(%s:%d) 0x%04X\n", file, line, errorCode);
    }
}

static bool CheckError(GLint obj, GLenum status, PFNGLGETSHADERIVPROC getiv, PFNGLGETSHADERINFOLOGPROC getInfoLog)
{
    GLint success;
    getiv(obj, status, &success);

    if (!success)
    {
        GLint length;
        getiv(obj, GL_INFO_LOG_LENGTH, &length);

        char *log = (char *)alloca(length);
        getInfoLog(obj, length, NULL, log);

        LOG_ERROR("Shader compile error for 0x%04X: %s\n", status, log);
        return false;
    }
    else
    {
        return true;
    }
}

GLint DebugRenderer::CompileShader(GLenum type, const char *source)
{
    GLint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // TODO: return placeholder shader instead?
    cpAssertHard(CheckError(shader, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog), "Error compiling shader");

    return shader;
}

GLint DebugRenderer::LinkProgram(GLint vshader, GLint fshader)
{
    GLint program = glCreateProgram();
    if (!glIsProgram(program))
    {
        LOG_ERROR("Failed to create shader program");
    }

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    // todo return placeholder program instead?
    cpAssertHard(CheckError(program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog), "Error linking shader program");

    return program;
}

bool DebugRenderer::ValidateProgram(GLint program)
{
    return glIsProgram(program);
}

void DebugRenderer::SetAttribute(GLuint program, char const *name, GLint size, GLenum gltype, GLsizei stride, GLvoid *offset)
{
    GLint index = glGetAttribLocation(program, name);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, gltype, GL_FALSE, stride, offset);
    CHECK_GL_ERRORS();
}

struct Vertex
{
    float x, y, z;
    float r, g, b, a;
};

void DebugRenderer::DrawDebugData()
{
    glBindVertexArray(vao);

    glUseProgram(program);
    int location = glGetUniformLocation(program, "u_matWorldViewProj");
    glUniformMatrix4fv(location, 1, GL_FALSE, worldViewProjMatrix._m);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    auto &vertices = DebugDraw::getInstance()->vertices;
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexXYZColor), vertices.data(), GL_STREAM_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    CHECK_GL_ERRORS();
}

void DebugRenderer::Init(void)
{
    // Setup the AA shader.
    GLint vshader = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLint fshader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    program = LinkProgram(vshader, fshader);
    CHECK_GL_ERRORS();

    // Setu VBO and VAO.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    CHECK_GL_ERRORS();
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    CHECK_GL_ERRORS();

    SetAttribute(program, "a_position", 3, GL_FLOAT, sizeof(VertexXYZColor), 0);
    SetAttribute(program, "a_color", 4, GL_FLOAT, sizeof(VertexXYZColor), (GLvoid*)(sizeof(float) * 3));
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS();
}
