#pragma once
#include <glad/glad.h>
#include <string>

class Shader
{
    public:
        Shader(const char* vsPath, const char* fsPath);
        ~Shader();
        std::string LoadShaderSource(const std::string& path);
};