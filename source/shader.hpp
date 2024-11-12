# pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <string>

class shader
{
private:

    GLuint m_ID;
    bool m_bIsInit = false;;

public:

    // *=*=*=*=*=*=*=*=*=*=
    //   Object Creation
    // *=*=*=*=*=*=*=*=*=*=

    shader();
    ~shader();

    shader(const shader&) = delete;
    shader(shader&&) = delete;
    shader& operator=(const shader&) = delete;
    shader& operator=(const shader&&) = delete; 

    // *=*=*=*=*=*=*=*=*=*=*=*
    //        Methods
    // *=*=*=*=*=*=*=*=*=*=*=*    

    void Init();
    void AttachShader(const std::string& path, GLenum type);
    void Link();
    void Use();
    void DeInitGL();

    GLuint GetID();

    // *=*=*=*=*=*=*=*=*=*=*=*
    //    UNIFORM SETTERS
    // *=*=*=*=*=*=*=*=*=*=*=*

    void set1b(const char* name, GLboolean value);
    void set1i(const char* name, GLint v0);
    void set1f(const char* name, GLfloat v0);
    void set1ui(const char* name, GLuint v0);
    void set2i(const char* name, GLint v0, GLint v1);
    void set2fv(const char* name, const glm::vec2& value);
    void set3f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2);
    void set3fv(const char* name, const glm::vec3& value);
    void set4fv(const char* name, const glm::vec4& value);
    void setM4fv(const char* name, GLboolean transpose, const glm::mat4& matrix);

    GLuint GetUniformBlockIndex(const char* uniformBlockName) const;
    void BindUniformBlockToBindingPoint(const char* uniformBlockName, const GLuint bindingPoint) const;
    
private:

    std::string ReadShaderCode(const std::string& codeFilePath);
};