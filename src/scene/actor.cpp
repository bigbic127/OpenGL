#include "actor.hpp"

void Actor::Update(float deltaTime)
{
    for(auto& c:components)
        c->Update(deltaTime);
}

void Actor::Render(CameraComponent* currentCamera, std::vector<LightComponent*> lights)
{
    for(auto& c:components)
        c->Render(currentCamera, lights);
}