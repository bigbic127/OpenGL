#pragma once
#include <memory>
#include <glm.hpp>
#include "renderer/mesh.hpp"
#include "renderer/material.hpp"

class IComponent
{
    public:
        virtual ~IComponent() = default;
        virtual void Update(const float deltaTime){};
        virtual void Render(){};
};

class SceneComponent:public IComponent
{
    private:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::vec4 quaternion;
};

class MeshComponent:public SceneComponent
{
    public:
        MeshComponent(std::shared_ptr<IMesh> m):mesh(m){}
        void SetMesh(std::shared_ptr<IMesh> m){mesh = m;}
        std::weak_ptr<IMesh> GetMesh() const{return mesh;}
        void SetMaterial(std::shared_ptr<Material> mat){material = mat;}
        std::weak_ptr<Material> GetMaterial() const{return material;}
        void Render() override;
    private:
        std::weak_ptr<IMesh> mesh;
        std::weak_ptr<Material> material;
};

class CameraComponent:public IComponent
{

};