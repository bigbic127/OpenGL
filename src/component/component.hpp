#pragma once
#include <memory>
#include <glm.hpp>
#include "mesh.hpp"
#include "material.hpp"

class IComponent
{
    public:
        virtual ~IComponent() = default;
        virtual void Update(float deltaTime){};
        virtual void Render(){};
};

class MeshComponent:public IComponent
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

class TransformComponent:public IComponent
{
    public:
        void Update(float deltaTime) override;
        glm::mat4 GetMatrix();
    private:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::vec4 quaternion;
};

class CameraComponent:public IComponent
{

};