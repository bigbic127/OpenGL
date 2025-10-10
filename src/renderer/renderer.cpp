#include "renderer.hpp"

void OpenGLRenderer::Init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void OpenGLRenderer::Clear()
{
    glClearColor(0.5,0.5,0.5,1.0);
}

void OpenGLRenderer::Begin()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::Render(World& world)
{
    for(const auto& actor:world.GetActors())
    {
        actor->Render(world.GetCurrentCamera(), world.GetLights());
    }
}

void OpenGLRenderer::End()
{

}