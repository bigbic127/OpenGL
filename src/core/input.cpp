#include "input.hpp"
#include "component/component.hpp"
#include "core/logger.hpp"

void Input::Init(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, MouseButton_CallbackStatic);
    glfwSetCursorPosCallback(window, CursorPos_CallbackStatic);
}

void Input::Process(GLFWwindow* window, float deltaTime)
{
    glm::vec3 position = camera->GetPosition();
    glm::vec3 front = camera->GetFront();
    glm::vec3 up = camera->GetUp();
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += cameraSpeed * front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= cameraSpeed * front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(front, up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        position += cameraSpeed * front;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        position -= cameraSpeed * front;
    if (camera != nullptr)
        camera->SetPosition(position);
}

void Input::MouseButton_Callback(GLFWwindow* window, int button, int action, int mods)
{
    if(glfwGetMouseButton(window, button) == GLFW_PRESS)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(action == GLFW_PRESS)
                bMouseLeftClicked = true;
        }
    }
    else
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(action == GLFW_RELEASE)
            {
                bMouseLeftClicked = false;
                bFirstClicked = false;
            }
        }        
    }
    std::cerr<<bMouseLeftClicked<<std::endl;
}

void Input::Scroll_Callback(GLFWwindow*window, double xoffset, double yoffset)
{

}

void Input::CursorPos_Callback(GLFWwindow* window, double xpos, double ypos)
{
    if(bMouseLeftClicked)
    {
        float yaw = camera->GetRotation().y;
        float pitch = camera->GetRotation().x;
        float x = static_cast<float>(xpos);
        float y = static_cast<float>(ypos);
        if (!bFirstClicked)
        {
            bFirstClicked = true;
            lastX = x;
            lastY = y;
        }
        float offsetX = x - lastX;
        float offsetY = lastY - y;
        lastX = x;
        lastY = y;
        float sensitivity = 0.1f;
        offsetX *= sensitivity;
        offsetY *= sensitivity;
        yaw += offsetX;
        pitch += offsetY;
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        camera->SetFront(front);
        camera->SetRotation(glm::vec3(pitch, yaw, 0.0f));
    }
}

void Input::MouseButton_CallbackStatic(GLFWwindow* window, int button, int action, int mods)
{
    Input* instance = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if(instance)
        instance->MouseButton_Callback(window, button, action, mods);
}

void Input::CursorPos_CallbackStatic(GLFWwindow* window, double xpos, double ypos)
{
    Input* instance = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if(instance)
        instance->CursorPos_Callback(window, xpos, ypos);
}

