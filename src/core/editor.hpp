#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>

class Window;
class ResourceManager;
class Input;
class IRenderer;

class Editor
{
    public:
        Editor(GLFWwindow* w):window(w){}
        ~Editor();
        void Init();
        void Update();
        void CreateIcon();
        void CreateLayout();
        void CreateStyle();
        GLuint LoadIconTexture(const std::string path);
        void SetInput(Input* i){input = i;}
        void SetResourceManager(ResourceManager* r){resourceManager = r;}
        void SetRenderer(IRenderer* r){renderer = r;}
    private:
        GLFWwindow* window;
        ResourceManager* resourceManager;
        IRenderer* renderer;
        Input* input;
        GLuint openFolderID;
};