#include "shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

shader::shader() 
{}

shader::~shader()
{}

std::string shader::ReadShaderCode(const std::string& codeFilePath)
{
    std::string code;
    std::ifstream shaderFile;
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        shaderFile.open(codeFilePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        code = shaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout 
            << "Shader code read error! \n\t[path]: " << codeFilePath
            << "\n\t[error mmessage]: " << e.what() 
            << std::endl;
    }

    return code;
}

void shader::Init()
{
    if (m_bIsInit) {
        std::cout << "The shader is already Init" << std::endl;
        return;
    }

    m_bIsInit = true;
    m_ID = glCreateProgram();
}

void shader::AttachShader(const std::string& path, GLenum type, bool embeddedString)
{
    if (!m_bIsInit) {
        std::cout 
            << "Shader must be initialized befor AttachShader call \n\t[path]: " << path 
            << "\n\t[type]: " << type 
            << std::endl;
        return;
    }

    GLuint sh = glCreateShader(type);
    std::string source = embeddedString ? path : ReadShaderCode(path);
    const char* source_c = source.c_str();
    glShaderSource(sh, 1, &source_c, NULL);
    glCompileShader(sh);

    GLint success;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint log_length;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> message(log_length);
        glGetShaderInfoLog(sh, log_length, nullptr, message.data());
        
        std::cout 
            << "AttachShader error! \n\t[path]: " << path 
            << "\n\t[type]: " << type 
            << "\n\t[error message]: \n" << message.data() << std::endl;
        return;
    }

    glAttachShader(m_ID, sh);
    glDeleteShader(sh);
}

void shader::Link()
{
    if (!m_bIsInit) {
        std::cout << "Shader must be initialized befor linking" << std::endl;
        return;
    }

    glLinkProgram(m_ID);

    GLint success; 
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint log_length;
        glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> message(log_length);
        glGetProgramInfoLog(m_ID, log_length, nullptr, message.data());

        std::cout << "LinkProgram error! \n\t[error message]: \n" << message.data() << std::endl;
        return;
    }
}

GLuint shader::GetID()
{
    return m_ID;
}

void shader::DeInitGL()
{
    if (!m_bIsInit) {
        std::cout << "Can not delete shader which wasn't initialized" << std::endl;
        return;
    }

    glDeleteShader(m_ID);
}

void shader::Use()
{
    glUseProgram(m_ID);
}

    // *=*=*=*=*=*=*=*=*=*=*=*
    //    UNIFORM SETTERS
    // *=*=*=*=*=*=*=*=*=*=*=*

void shader::set1b(const char* name, GLboolean value)
{
    glUniform1i(glGetUniformLocation(m_ID, name), (int)value); 
}

void shader::set1i(const char* name, GLint v0)
{
    glUniform1i(glGetUniformLocation(m_ID, name), v0);
}

void shader::set1f(const char* name, GLfloat v0)
{
    glUniform1f(glGetUniformLocation(m_ID, name), v0);
}

void shader::set1ui(const char* name, GLuint v0)
{
    glUniform1ui(glGetUniformLocation(m_ID, name), v0);
}

void shader::set2i(const char* name, GLint v0, GLint v1)
{
    glUniform2i(glGetUniformLocation(m_ID, name), v0, v1);
}

void shader::set2fv(const char* name, const glm::vec2& value)
{
    glUniform2fv(glGetUniformLocation(m_ID, name), 1, (float*)&value);
}

void shader::set3f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2)
{
    glUniform3f(glGetUniformLocation(m_ID, name), v0, v1, v2);
}

void shader::set3fv(const char* name, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(m_ID, name), 1, (float*)&value);
}

void shader::set4fv(const char* name, const glm::vec4& value)
{
    glUniform4fv(glGetUniformLocation(m_ID, name), 1, (float*)&value);
}

void shader::setM4fv(const char* name, GLboolean transpose, const glm::mat4& matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, transpose, (float*)&matrix);
}

GLuint shader::GetUniformBlockIndex(const char* uniformBlockName) const
{
    GLuint result = glGetUniformBlockIndex(m_ID, uniformBlockName);
    if (result == GL_INVALID_INDEX) {
        std::cout << "Could not get index of uniform block [" << uniformBlockName << "], check if suck uniform block really exists" << std::endl;  
    }

    return result;
}

void shader::BindUniformBlockToBindingPoint(const char* uniformBlockName, const GLuint bindingPoint) const
{
    const GLuint blockIndex = GetUniformBlockIndex(uniformBlockName);
    if (blockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(m_ID, blockIndex, bindingPoint);
    } 
}

