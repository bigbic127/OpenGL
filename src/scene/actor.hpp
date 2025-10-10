#pragma once
#include <string>
#include <vector>
#include <memory>
#include "component/component.hpp"

class Actor
{
    public:
        Actor(const std::string& n = "Actor"):name(n){}
        std::string name;
        template<typename T, typename... Args>
            T* AddComponent(Args&&... args)
            {
                auto comp = std::make_unique<T>(std::forward<Args>(args)...);
                T* ptr = comp.get();
                components.push_back(std::move(comp));
                return ptr;
            }
        template<typename T>
            T* GetComponent()
            {
                for(auto& c:components)
                {
                    if(auto casted = dynamic_cast<T*>(c.get()))
                        return casted;
                }
                return nullptr;
            }
        template<typename T>
            std::vector<T*> GetComponents()
            {
                std::vector<T*> result;
                for (auto& c:components)
                {
                    if(auto casted = dynamic_cast<T*>(c.get()))
                        result.push_back(casted);
                }
                return result;                
            }
        template<typename T>
            T* FindComponent(T* comp)
            {
                for(auto& c:components)
                {
                    if(auto casted = dynamic_cast<T*>(c.get()))
                    {
                        if(casted == comp)
                            return casted;
                    }
                }
                return nullptr;
            }
        void Update(float deltaTime);
        void Render(CameraComponent* currentCamera, std::vector<LightComponent*> lights);
    private:
        std::vector<std::unique_ptr<IComponent>> components;
};
