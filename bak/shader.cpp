#include "shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const char* vsPath, const char* fsPath)
{
    unsigned int vsID, fsID;
    std::string vsCode = LoadShaderSource(vsPath);
    std::string fsCode = LoadShaderSource(fsPath);
    const char* vsSource = vsCode.c_str();
    const char* fsSource = fsCode.c_str();
    vsID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsID, 1, &vsSource, nullptr);
    glCompileShader(vsID);
    CheckCompileError(vsID, "VERTEX");
    fsID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fsID, 1, &fsSource, nullptr);
    glCompileShader(fsID);
    CheckCompileError(fsID, "FRAGMENT");
    pID = glCreateProgram();
    glAttachShader(pID, vsID);
    glAttachShader(pID, fsID);
    glLinkProgram(pID);
    CheckCompileError(pID, "PROGRAM");
    glDeleteShader(vsID);
    glDeleteShader(fsID);
}

Shader::~Shader()
{
    glDeleteProgram(pID);
}

void Shader::UseProgram()
{
    glUseProgram(pID);
}

std::string Shader::LoadShaderSource(const std::string& path)
{
    std::ifstream file(path);
    if(!file.is_open())
    {
        std::cerr << "Failed to open shader file : " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

void Shader::CheckCompileError(unsigned int id, std::string type)
{
    int success;
    char infoLog[512];
    if(type != "PROGRAM")
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(id, 512, nullptr, infoLog);
            std::cerr << "Failed to " << type << " Shader Compile.\nLog: " << infoLog << std::endl;
        }
    }else
    {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(id, 512, nullptr, infoLog);
            std::cerr << "Failed to Program Link.\nLog: " << infoLog << std::endl;
        }
    }
}