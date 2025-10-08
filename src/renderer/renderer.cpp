#include "renderer.hpp"

void OpenGLRenderer::Clear()
{
    glClearColor(0.5,0.5,0.5,1.0);
}

void OpenGLRenderer::Begin()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderer::Render(World& world)
{
    for(const auto& actor:world.GetActors())
    {
        actor->Update(world.GetDeltaTime());
        actor->Render();
    }
}

void OpenGLRenderer::End()
{

}