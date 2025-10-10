#pragma once
#include <memory>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include "renderer/mesh.hpp"
#include "renderer/material.hpp"

struct Transform
{
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

class IComponent
{
    public:
        virtual ~IComponent() = default;
        virtual void Update(const float deltaTime){};
        virtual void Render(){};
};

class SceneComponent:public IComponent
{
    public:
        void SetTransform(Transform t){transform = t;ToQuaternion();}
        void SetPosition(glm::vec3 p){transform.position = p;}
        void SetRotation(glm::vec3 r){transform.rotation = r;ToQuaternion();}
        void SetScale(glm::vec3 s){transform.scale = s;}
        Transform GetTransform() const{return transform;}
        glm::vec3 GetPosition() const{return transform.position;}
        glm::vec3 GetRotation() const{return transform.rotation;}
        glm::vec3 GetScale() const{return transform.scale;}
        glm::mat4 GetMatrix() const;
    protected:
        Transform transform;
        void ToQuaternion();
        void ToEuler();
    private:
        glm::quat quaternion{1.0f, 0.0f, 0.0f, 0.0f};
};
class CameraComponent;
class LightComponent;
class MeshComponent:public SceneComponent
{
    public:
        MeshComponent(std::shared_ptr<IMesh> m):mesh(m){}
        void SetMesh(std::shared_ptr<IMesh> m){mesh = m;}
        std::weak_ptr<IMesh> GetMesh() const{return mesh;}
        void SetMaterial(std::shared_ptr<Material> mat){material = mat;}
        std::weak_ptr<Material> GetMaterial() const{return material;}
        void SetCameraComponent(CameraComponent* component){cameraComponent = component;}
        void SetLightComponent(LightComponent* component){lightComponent = component;}
        void Render() override;
    private:
        std::weak_ptr<IMesh> mesh;
        std::weak_ptr<Material> material;
        CameraComponent* cameraComponent = nullptr;
        LightComponent* lightComponent =  nullptr;
        
};

class CameraComponent:public SceneComponent
{
    public:
        glm::mat4 GetViewMatrix()const{return view;}
        glm::mat4 GetProjectionMatrix()const{return projection;}
        void SetAspect(float a){aspect = a;}
        void Update(float deltaTime) override;
    private:
        glm::mat4 view{1.0f};
        glm::mat4 projection{1.0f};
        float fov{45.0f};
        glm::vec3 target{0.0f, 0.0f, 0.0f};
        glm::vec3 front{0.0f, 0.0f, -1.0f};
        glm::vec3 up{0.0f, 1.0f, 0.0f};
        float aspect = 1.778f;
        float near = 0.01f;
        float far = 1000.0f;
};

class LightComponent:public SceneComponent
{
    public:
        void SetIntensity(float value){lightIntensity = value;}
        void SetColor(glm::vec3 c){color = c;}
        float GetIntensity() const{return lightIntensity;}
        glm::vec3 GetColor() const{return color;}
    private:
        float lightIntensity = 1.0f;
        glm::vec3 color{1.0f, 1.0f, 1.0f};
};

class DirectionalLightComponent:public LightComponent
{

};