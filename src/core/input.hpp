#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class CameraComponent;

class Input
{
    public:
        void Process(GLFWwindow* window, float deltaTime);
        void SetCameraComponent(CameraComponent* c){camera = c;}
        void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        void ScrollCallback(GLFWwindow*window, double xoffset, double yoffset);
        void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    private:
        CameraComponent* camera;
        bool bMouseLeftClicked;
        bool bFirstClicked;
        float lastX, lastY;
        
};