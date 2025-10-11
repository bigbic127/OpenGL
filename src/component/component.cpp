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

glm::vec3 SceneComponent::GetDirection() const
{
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 fRot = rotZ * rotX * rotY;
    glm::vec3 dir = glm::vec3(fRot * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

    return dir;
}

void MeshComponent::Render(CameraComponent* currentCamera, std::vector<LightComponent*> lights)
{
    if(auto m = mesh.lock())
    {
        m->Bind();
        if(auto mat = material.lock())
        {
            mat->SetModelMatrix(GetMatrix());
            //camera
            mat->SetViewMatrix(currentCamera->GetViewMatrix());
            mat->SetProjectMatrix(currentCamera->GetProjectionMatrix());
            //light
            for(auto& light:lights)
            {
                mat->SetLightPosition(light->GetPosition());
                mat->SetLightDirectional(light->GetDirection());
                mat->SetLightIntensity(light->GetIntensity());
                mat->SetLightColor(light->GetColor());
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