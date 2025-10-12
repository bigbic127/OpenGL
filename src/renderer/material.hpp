#pragma once
#include <memory>
#include <vector>
#include "shader.hpp"
#include "texture.hpp"

struct MaterialParameter
{
    glm::vec3 ambientColor = {0.0f, 0.0f, 0.0f};
    glm::vec3 baseColor = {1.0f, 1.0f, 1.0f};
    glm::vec3 specularColor = {0.5f, 0.5f, 0.5f};
    float specularShininess = 32.0f;
    bool bDiffuse = false;
    std::weak_ptr<Texture> diffuseTexture;
    bool bSpecular = false;
    std::weak_ptr<Texture> specularTexture;
    bool bNormal = false;
    std::weak_ptr<Texture> normalTexture;
};

class IMaterial
{
    public:
        ~IMaterial() = default;
        virtual void Apply() = 0;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
};

class Material:public IMaterial
{
    public:
        Material(std::shared_ptr<Shader> s):shader(s){}
        void SetModelMatrix(glm::mat4 m){mModel = m;}
        void SetViewMatrix(glm::mat4 m){mView = m;}
        void SetProjectMatrix(glm::mat4 m){mProjection = m;}
        void SetLightPosition(glm::vec3 v){vLightPosition = v;}
        void SetLightDirectional(glm::vec3 v){vLightDirectional = v;}
        void SetLightColor(glm::vec3 v){vLightColor = v;}
        void SetLightIntensity(float v){fLightIntensity =v;}
        void SetCameraPosition(glm::vec3 v){vCameraPosition = v;}
        void AddTexture(std::shared_ptr<Texture> texture, std::string name);
        void Apply() override;
        void Bind() override;
        void Unbind() override;
        //parameter
        void SetParameter(MaterialParameter p){parameter = p;}
        void SetAmbientColor(glm::vec3 color){parameter.ambientColor = color;}
        void SetBaseColor(glm::vec3 color){parameter.baseColor = color;}
    private:
        std::weak_ptr<Shader> shader;
        MaterialParameter parameter;
        std::vector<std::weak_ptr<Texture>> textures;
        glm::mat4 mModel = glm::mat4(1.0f);
        glm::mat4 mView = glm::mat4(1.0f);
        glm::mat4 mProjection = glm::mat4(1.0f);
        glm::vec3 vLightPosition = glm::vec3(0.0f);
        glm::vec3 vLightDirectional = glm::vec3(1.0f, 1.0f, -1.0f);
        glm::vec3 vLightColor = glm::vec3(1.0f);
        glm::vec3 vCameraPosition = glm::vec3(0.0f);
        float fLightIntensity = 1.0f;
};