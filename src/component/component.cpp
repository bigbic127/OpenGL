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
            mat->Apply();
        }
        m->Draw();
        m->Unbind();
    }
}

glm::mat4 CameraComponent::GetViewMatrix()
{
    glm::mat4 view(1.0f);
    view = glm::lookAt(transform.position, target, up);
    return view;
}

glm::mat4 CameraComponent::GetProjectionMatrix()
{
    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(fov), aspect, near, far);
    return projection;
}