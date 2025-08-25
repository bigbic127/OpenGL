#include <glad/glad.h>
#include <glfw/glfw3.h>

const GLint WIDTH = 800;
const GLint HEIGHT = 600;

const GLchar* vertexShaderSource = R"(
    #version 450 core
    int vec4 vertexPos;
    void main()
    {
        gl_Position = vertexPos;
    }
)";

const GLchar* fragmentShaderSource = R"(
    #version 450 core
    out vec4 fragColor;
    void main()
    {
        fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
)";

const GLfloat vertices[] = 
{
    -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f,
    0.0f,  0.5f, 0.0f, 1.0f
};

void DrawTriangle()
{
    
}

int main()
{
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        glfwDestroyWindow(window);
        return -1;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        glfwDestroyWindow(window);
        return -1;
    }

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f,0.5f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    glfwDestroyWindow(window);
    return 0;
}