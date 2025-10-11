#include "world.hpp"
#include "GLFW/glfw3.h"
#include "core/logger.hpp"

Actor* World::CreateActor()
{
    auto actor = std::make_unique<Actor>();
    Actor* ptr = actor.get();
    actors.push_back(std::move(actor));
    return ptr;
}

void World::DeleteActor(Actor* actor)
{
    actors.erase(
        std::remove_if(actors.begin(), actors.end(),
        [actor](const std::unique_ptr<Actor>& a)
        {
            return a.get() == actor; 
        }),
        actors.end()
    );
}

void World::Update()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    for(const auto& actor:GetActors())
    {
        actor->Update(GetDeltaTime());
    }
}