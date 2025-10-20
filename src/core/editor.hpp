#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Window;

class Editor
{
    public:
        Editor(GLFWwindow* w):window(w){}
        ~Editor();
        void Init();
        void Update();
        void CreateLayout();
        void TopMenuBar();
        void CreateStyle();
    private:
        GLFWwindow* window;

};