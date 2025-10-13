#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class CameraComponent;

class Input
{
    public:
        void Init(GLFWwindow* window);
        void Process(GLFWwindow* window, float deltaTime);
        void SetCameraComponent(CameraComponent* c){camera = c;}
        static void MouseButton_CallbackStatic(GLFWwindow* window, int button, int action, int mods);
        static void CursorPos_CallbackStatic(GLFWwindow* window, double xpos, double ypos);
        void MouseButton_Callback(GLFWwindow* window, int button, int action, int mods);
        void Scroll_Callback(GLFWwindow*window, double xoffset, double yoffset);
        void CursorPos_Callback(GLFWwindow* window, double xpos, double ypos);
    private:
        CameraComponent* camera;
        bool bMouseLeftClicked;
        bool bFirstClicked;
        float lastX, lastY;
        
};