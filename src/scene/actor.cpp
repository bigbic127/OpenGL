#include "actor.hpp"

void Actor::Update(float deltaTime)
{
    for(auto& c:components)
        c->Update(deltaTime);
}

void Actor::Render()
{
    for(auto& c:components)
        c->Render();
}