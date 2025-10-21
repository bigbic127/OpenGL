#include "renderer.hpp"
#include "core/window.hpp"
#include "core/logger.hpp"


OpenGLRenderer::~OpenGLRenderer()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &cbo);
}

void OpenGLRenderer::Init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CreateBuffer(800, 600);
}

void OpenGLRenderer::Clear()
{
    glClearColor(0.5,0.5,0.5,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::Begin()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void OpenGLRenderer::Render(std::weak_ptr<World> world)
{
    if(auto w = world.lock())
    {
        for(const auto& actor:w->GetActors())
        {
            actor->Render(w->GetCurrentCamera(), w->GetLights());
        }
    }
}

void OpenGLRenderer::End()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::CreateBuffer(int w, int h)
{
    width = w;
    height = h;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //Color
    glGenTextures(1, &cbo);
    glBindTexture(GL_TEXTURE_2D, cbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cbo, 0);
    //depth stencill
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::ErrorMessage("Framebuffer not complete.");
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool OpenGLRenderer::ResizeBuffer(int w, int h)
{
    if(w <=0 || h <=0) return false;
    if(w == width && h == height) return true;
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &cbo);
    glDeleteRenderbuffers(1, &rbo);
    CreateBuffer(w, h);
}