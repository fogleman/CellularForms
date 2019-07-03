#define GL_GLEXT_PROTOTYPES
#include "program.h"

#include <exception>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/string_cast.hpp>

namespace
{

GLuint compileShader(GLenum shaderType, std::string shaderSource)
{
    const GLuint shader = glCreateShader(shaderType);
    const GLchar *source = shaderSource.c_str();
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled)
    {
        GLint maxLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
        glDeleteShader(shader);
        throw std::runtime_error(std::string(infoLog.begin(), infoLog.end()));
    }
    return shader;
}

GLuint compileProgram(std::string vertexSource, std::string fragmentSource)
{
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (!isLinked)
    {
        GLint maxLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw std::runtime_error(std::string(infoLog.begin(), infoLog.end()));
    }
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

} // namespace

Program::Program(std::string vertexSource, std::string fragmentSource) : m_Program(compileProgram(vertexSource, fragmentSource))
{
}

void Program::Use() const
{
    glUseProgram(m_Program);
}

GLint Program::GetUniformLocation(std::string name) const
{
    return glGetUniformLocation(m_Program, name.c_str());
}

GLint Program::GetAttribLocation(std::string name) const
{
    return glGetAttribLocation(m_Program, name.c_str());
}
