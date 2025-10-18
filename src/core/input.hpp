#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "resource/resource.hpp"

class CameraComponent;

class Input
{
    public:
        void Process(GLFWwindow* window, float deltaTime);
        void SetCameraComponent(CameraComponent* c){camera = c;}
        void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        void ScrollCallback(GLFWwindow*window, double xoffset, double yoffset);
        void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        void SetResourceManager(ResourceManager* r){resourceManager = r;}
    private:
        CameraComponent* camera;
        ResourceManager* resourceManager;
        bool bMouseLeftClicked;
        bool bFirstClicked;
        float lastX, lastY;
        
};