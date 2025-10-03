#pragma once
#include <glad/glad.h>
#include "mesh.hpp"

class IRenderer
{
    public:
        virtual ~IRenderer() = default;
        virtual void Clear() = 0;
        virtual void Begin() = 0;
        virtual void Render(Mesh& mesh) = 0;
        virtual void End() = 0;
};

class OpenGLRenderer:public IRenderer
{
    public:
        void Clear() override;
        void Begin() override;
        void Render(Mesh& mesh) override;
        void End() override;
};

class VulkanRenderer:public IRenderer
{
    //추가적인 학습 필요.
};