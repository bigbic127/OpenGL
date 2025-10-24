#pragma once
#include <memory>
#include <vector>
#include "actor.hpp"

class Editor;

class World
{
    public:
        Actor* CreateActor(std::string name = "Actor");
        void DeleteActor(Actor* actor);
        const std::vector<std::unique_ptr<Actor>>& GetActors(){return actors;}
        const float GetDeltaTime()const{return deltaTime;}
        CameraComponent* GetCurrentCamera()const{return camera;}
        std::vector<LightComponent*> GetLights()const{return lights;}
        void SetCurrentCamera(CameraComponent* c){camera = c;}
        void AddLight(LightComponent* l){lights.push_back(l);}
        void SetEditor(Editor* e){editor= e;}
        Editor* GetEditor()const{return editor;}
        void Update();
    private:
        std::vector<std::unique_ptr<Actor>> actors;
        float deltaTime;
        float lastFrame;
        CameraComponent* camera =  nullptr;
        std::vector<LightComponent*> lights;
        Editor* editor = nullptr;
        unsigned int id = 0;
};