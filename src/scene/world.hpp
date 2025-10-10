#pragma once
#include <memory>
#include <vector>
#include "actor.hpp"

class World
{
    public:
        Actor* CreateActor();
        void DeleteActor(Actor* actor);
        const std::vector<std::unique_ptr<Actor>>& GetActors(){return actors;}
        const float GetDeltaTime()const{return deltaTime;}
        void SetDeltaTime(float time){deltaTime = time;}
        CameraComponent* GetCurrentCamera()const{return camera;}
        std::vector<LightComponent*> GetLights()const{return lights;}
        void SetCurrentCamera(CameraComponent* c){camera = c;}
        void AddLight(LightComponent* l){lights.push_back(l);}
        void Update();
    private:
        std::vector<std::unique_ptr<Actor>> actors;
        float deltaTime;
        CameraComponent* camera =  nullptr;
        std::vector<LightComponent*> lights;
};