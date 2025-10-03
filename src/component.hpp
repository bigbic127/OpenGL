#pragma once
#include <memory>
#include <glm.hpp>
#include "mesh.hpp"

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
        std::shared_ptr<IMesh> GetMesh() const {return mesh;}
        void Render() override;
    private:
        std::shared_ptr<IMesh> mesh;
};

class TransformComponent:public IComponent
{
    public:
        void Update(float deltaTime) override;
    private:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::vec4 quaternion;
};