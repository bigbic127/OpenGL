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
    private:
        std::vector<std::unique_ptr<Actor>> actors;
        float deltaTime;
};