#include "window.hpp"
#include "logger.hpp"

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::Init()
{
    if(!glfwInit())
    {
        Logger::ErrorMessage("Faield To Glfw Init.");
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    window = glfwCreateWindow(width, height, "OpengGL", nullptr, nullptr);
    if (window == nullptr)
    {
        Logger::ErrorMessage("Failed to Create Window.");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetErrorCallback(Logger::WindowErrorCallback);
    glfwSetWindowSizeCallback(window, Window::WindowResizeCallback);
    //glfwSetFramebufferSizeCallback(window, Window::FrameBufferSizeCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    if(!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
    {
        Logger::ErrorMessage("Failed to Glad Load.");
        return false;
    }
    glfwSwapInterval(1);
    return true;
}

void Window::SwapBuffer()
{
    glfwSwapBuffers(window);
}

void Window::PollEvent()
{
    glfwPollEvents();
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void Window::WindowResizeCallback(GLFWwindow* window, int w, int h)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->width = w;
    win->height = h;
}

void Window::FrameBufferSizeCallback(GLFWwindow* window, int w, int h)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    //glViewport(0, 0, w, h);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(win->input !=nullptr)
        win->input->MouseButtonCallback(window, button, action, mods);
}

void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(win->input !=nullptr)
        win->input->CursorPosCallback(window, xpos, ypos);
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    
}

float Window::GetAspect()
{
    glfwGetFramebufferSize(window, &width, &height);
    return float(width)/height;
}