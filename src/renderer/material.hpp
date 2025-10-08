#pragma once
#include <memory>
#include "shader.hpp"

class IMaterial
{
    public:
        ~IMaterial() = default;
        virtual void Apply() = 0;
};

class Material:public IMaterial
{
    public:
        Material(std::shared_ptr<Shader> s):shader(s){}
        void Apply() override;
    private:
        std::weak_ptr<Shader> shader;
        glm::mat4 mModel = glm::mat4(1.0);
        glm::mat4 mView = glm::mat4(1.0);
        glm::mat4 mProjection = glm::mat4(1.0);
        glm::vec3 baseColor = glm::vec3(1.0);
};