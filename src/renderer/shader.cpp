#include "shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <gtc/type_ptr.hpp>
#include "core/logger.hpp"
#include "system/folder.hpp"

Shader::Shader(const std::string& vsPath, const std::string& fsPath)
{
    unsigned int vID, fID;
    std::string vsSource = LoadShaderSource(vsPath);
    std::string fsSource = LoadShaderSource(fsPath);
    const char* vsCode = vsSource.c_str();
    const char* fsCode = fsSource.c_str();
    vID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vID, 1, &vsCode, nullptr);
    glCompileShader(vID);
    Logger::CompileShaderError(vID, "VERTEX");
    fID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fID, 1, &fsCode, nullptr);
    glCompileShader(fID);
    Logger::CompileShaderError(fID, "FRAGMENT");
    programID = glCreateProgram();
    glAttachShader(programID, vID);
    glAttachShader(programID, fID);
    glLinkProgram(programID);
    Logger::CompileShaderError(programID, "PROGRAM");
    glDeleteShader(vID);
    glDeleteShader(fID);
}

Shader::~Shader()
{
    glDeleteProgram(programID);
}

void Shader::UseProgram()
{
    glUseProgram(programID);
}

std::string Shader::LoadShaderSource(const std::string& path)
{
    std::string p = path;
    #ifdef __APPLE__
        std::string root = GetExecutableDir();
        p = root + path;
    #endif
    std::ifstream file(p);
    if(!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << path << std::endl;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

GLint Shader::GetLocation(const std::string& name) const
{
    return glGetUniformLocation(programID, name.c_str());
}

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1d(GetLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(GetLocation(name), value);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1d(GetLocation(name), value);
}

void Shader::SetVector3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(GetLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMatrix4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}