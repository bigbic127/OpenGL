#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
    public:
        Window(int w = 1920, int h = 1080):width(w), height(h){}
        ~Window();
        bool Init();
        bool ShouldClose();
        void SwapBuffer();
        void PollEvent();
        //Callback
        static void WindowResizeCallback(GLFWwindow* window, int w, int h);
        static void FrameBufferSizeCallback(GLFWwindow* window, int w, int h);
        float GetAspect();
    private:
        int width, height;
        GLFWwindow* window = nullptr;
};