#include "world.hpp"

Actor* World::CreateActor()
{
    auto actor = std::make_unique<Actor>();
    Actor* ptr = actor.get();
    actors.push_back(std::move(actor));
    return ptr;
}