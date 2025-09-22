#pragma once
#include <glad/glad.h>
#include <string>

class Shader
{
    public:
        Shader(const char* vsPath, const char* fsPath);
        ~Shader();
        void UseProgram();
    protected:
        std::string LoadShaderSource(const std::string& path);
        void CheckCompileError(unsigned int id, std::string type);
    private:
        unsigned int pID;
};