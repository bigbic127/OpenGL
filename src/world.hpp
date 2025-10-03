#pragma once
#include <memory>
#include <vector>
#include "actor.hpp"

class World
{
    public:
        Actor* CreateActor();
    private:
        std::vector<std::unique_ptr<Actor>> actors;
};