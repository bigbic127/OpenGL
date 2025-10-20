#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "input.hpp"
#include "editor.hpp"

class Window
{
    public:
        Window(int w = 1280, int h = 720):width(w), height(h){}
        ~Window();
        bool Init();
        bool ShouldClose();
        void SwapBuffer();
        void PollEvent();
        //Callback
        static void WindowResizeCallback(GLFWwindow* window, int w, int h);
        static void FrameBufferSizeCallback(GLFWwindow* window, int w, int h);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        void SetInput(Input* i){input = i;}
        void SetEditor(Editor* e){editor = e;}
        float GetAspect();
        void GetSize(int& w, int& h){w = width;h=height;}
        GLFWwindow* GetWindow()const{return window;}
    private:
        int width, height;
        GLFWwindow* window;
        Editor* editor;
        Input* input;
};