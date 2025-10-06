#include "shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
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