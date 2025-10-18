#pragma once
#include <glad/glad.h>
#include <string>
#include <glm.hpp>

class IShader
{
    public:
        ~IShader() = default;
        virtual void UseProgram() = 0;
};

class Shader:public IShader
{
    public:
        Shader(const std::string& vsPath, const std::string& fsPath);
        ~Shader();
        std::string LoadShaderSource(const std::string& path);
        void UseProgram() override;
        void Unbind();
        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVector3(const std::string& name, const glm::vec3& value) const;
        void SetMatrix4(const std::string& name, const glm::mat4& value) const;
        GLint GetLocation(const std::string& name) const;
    private:
        unsigned int programID;
};