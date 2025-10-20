#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "resource/resource.hpp"

class CameraComponent;

class Input
{
    public:
        Input(ResourceManager* r):resourceManager(r){}
        void Process(GLFWwindow* window, float deltaTime);
        void SetCameraComponent(CameraComponent* c){camera = c;}
        void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        void ScrollCallback(GLFWwindow*window, double xoffset, double yoffset);
    private:
        CameraComponent* camera;
        ResourceManager* resourceManager;
        bool bMouseLeftClicked = false, bMouseRightClicked, bMouseMiddleClicked;
        bool bFirstClicked = false;
        float lastX, lastY;
        
};