#include "component.hpp"
#include <iostream>
glm::mat4 SceneComponent::GetMatrix() const
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, transform.position);
    model *= glm::mat4_cast(quaternion);
    model = glm::scale(model, transform.scale);
    return model;
}
void SceneComponent::ToQuaternion()
{
    quaternion = glm::normalize(glm::quat(glm::radians(transform.rotation)));
//    quaternion = glm::angleAxis(glm::radians(transform.rotation.y),glm::vec3(0,1,0))*
//                 glm::angleAxis(glm::radians(transform.rotation.x),glm::vec3(1,0,0))*
//                 glm::angleAxis(glm::radians(transform.rotation.z),glm::vec3(0,0,1));
}

void SceneComponent::ToEuler()
{
    transform.rotation = glm::degrees(glm::eulerAngles(glm::normalize(quaternion)));
}

void MeshComponent::Render()
{
    if(auto m = mesh.lock())
    {
        m->Bind();
        if(auto mat = material.lock())
        {
            mat->SetModelMatrix(GetMatrix());
            if(cameraComponent != nullptr)
            {
                mat->SetViewMatrix(cameraComponent->GetViewMatrix());
                mat->SetProjectMatrix(cameraComponent->GetProjectionMatrix());
            }
            if(lightComponent != nullptr)
            {
                mat->SetLightPosition(lightComponent->GetPosition());
                mat->SetLightIntensity(lightComponent->GetIntensity());
                mat->SetLightColor(lightComponent->GetColor());
            }
            mat->Bind();
        }
        m->Draw();
        m->Unbind();
        if(auto mat = material.lock())
        {
            mat->Unbind();
        }

    }
}

void CameraComponent::Update(float deltaTime)
{
    view = glm::lookAt(transform.position, target, up);
    projection = glm::perspective(glm::radians(fov), aspect, near, far);
}