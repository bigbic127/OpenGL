#pragma once
#include <memory>
#include <glad/glad.h>
#include "mesh.hpp"
#include "scene/world.hpp"

class Window;
class IRenderer
{
    public:
        virtual ~IRenderer() = default;
        virtual void Init() = 0;
        virtual void Clear() = 0;
        virtual void Begin() = 0;
        virtual void Render(std::weak_ptr<World> world) = 0;
        virtual void End() = 0;
        virtual unsigned int GetFrameBuffer()const = 0;
        virtual bool ResizeBuffer(int w, int h) = 0;
        virtual float GetAspect() const = 0;
};

class OpenGLRenderer:public IRenderer
{
    public:
        ~OpenGLRenderer();
        void Init() override;
        void Clear() override;
        void Begin() override;
        void Render(std::weak_ptr<World> world) override;
        void End() override;
        unsigned int GetFrameBuffer()const override{return cbo;}
        bool ResizeBuffer(int w, int h) override;
        float GetAspect() const override {return float(width)/height;}
    private:
        void CreateBuffer(int w, int h);
        unsigned int fbo, rbo, cbo;
        int width, height;
};

class VulkanRenderer:public IRenderer
{
    //추가적인 학습 필요.
};