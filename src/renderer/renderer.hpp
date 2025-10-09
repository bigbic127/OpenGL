#pragma once
#include <glad/glad.h>
#include "mesh.hpp"
#include "scene/world.hpp"

class IRenderer
{
    public:
        virtual ~IRenderer() = default;
        virtual void Init() = 0;
        virtual void Clear() = 0;
        virtual void Begin() = 0;
        virtual void Render(World& world) = 0;
        virtual void End() = 0;
};

class OpenGLRenderer:public IRenderer
{
    public:
        void Init() override;
        void Clear() override;
        void Begin() override;
        void Render(World& world) override;
        void End() override;
};

class VulkanRenderer:public IRenderer
{
    //추가적인 학습 필요.
};