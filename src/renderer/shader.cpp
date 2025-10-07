#include "shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <gtc/type_ptr.hpp>
#include "core/logger.hpp"

Shader::Shader(const char* vsPath, const char* fspath)
{
    unsigned int vID, fID;
    const char* vsSource = LoadShaderSource(vsPath).c_str();
    const char* fsSource = LoadShaderSource(fspath).c_str();
    vID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vID, 1, &vsSource, nullptr);
    glCompileShader(vID);
    Logger::CompileShaderError(vID, "VERTEX");
    fID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fID, 1, &fsSource, nullptr);
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
    std::ifstream file(path);
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

void Shader::SetVector3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(GetLocation(name), 1, glm::value_ptr(value));
}