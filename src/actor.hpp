#pragma once
#include <string>
#include <vector>
#include <memory>
#include "component.hpp"

class Actor
{
    public:
        Actor(const std::string& n = "Actor"):name(n){}
        std::string name;
        std::vector<std::unique_ptr<IComponent>> components;
        template<typename T, typename... Args>
            T* AddComponent(Args&&... args);
        template<typename T>
            T* GetComponent();
        void Update(float deltaTime);
        void Render();

};
