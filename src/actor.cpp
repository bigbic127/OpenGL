#include "actor.hpp"

template<typename T, typename... Args>
T* Actor::AddComponent(Args&&... args)
{
    auto comp = std::make_unique<T>(std::forawrd<Args>(args)...);
    T* ptr = comp.get();
    components.push_back(std::move(comp));
    return ptr;
}

template<typename T>
T* Actor::GetComponent()
{
    for(auto& c:components)
    {
        if(auto casted = dynamic_cast<T*>(c.get()))
            return casted;
    }
    return nullptr;
}

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