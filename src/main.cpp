#include <glad/glad.h>
#include "mesh.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

int WIDTH = 800;
int HEIGHT = 600;

void ErrorCallback(int error_code, const char* description);
void WindowSizeCallback(GLFWwindow* window, int width, int height);
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);

std::vector<Vertex> cubeVertices = {
    // Front face
    {{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
    // Back face
    {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
    // Left face
    {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
    {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
    {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
    // Right face
    {{ 0.5f, -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
    {{ 0.5f,  0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
    // Top face
    {{-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
    {{-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
    // Bottom face
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
    {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
};
std::vector<unsigned int> cubeIndices = {
    // Front face
    0, 1, 2,  2, 3, 0,
    // Back face
    4, 5, 6,  6, 7, 4,
    // Left face
    8, 9,10, 10,11, 8,
    // Right face
   12,13,14, 14,15,12,
    // Top face
   16,17,18, 18,19,16,
    // Bottom face
   20,21,22, 22,23,20
};


class Shader
{
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        ~Shader();
        std::string LoadShaderSource(const std::string& filePath);
        void UseProgram();
        void CheckCompileError(int id, std::string type);
    private:
        int vertexID, fragmentID, programID;
    
};

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode = LoadShaderSource(vertexPath);
    std::string fragmentCode = LoadShaderSource(fragmentPath);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();
    
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vertexSource, nullptr);
    glCompileShader(vertexID);
    CheckCompileError(vertexID, "VERTEX");
    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentID);
    CheckCompileError(fragmentID, "FRAGMENT");
    programID = glCreateProgram();
    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);
    glLinkProgram(programID);
    CheckCompileError(programID, "PROGRAM");
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
}

Shader::~Shader()
{
    glDeleteProgram(programID);
}

std::string Shader::LoadShaderSource(const std::string& filePath)
{
    std::ifstream file(filePath);
    if(!file.is_open())
    {
        std::cerr << "Failed to open shader file : " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

void Shader::UseProgram()
{
    glUseProgram(programID);
}

void Shader::CheckCompileError(int id, std::string type)
{
    int success;
    char infoLog[512];
    if (type != "PROGRAM")
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(id, 512, nullptr, infoLog);
            std::cerr << "Failed to " << type << " Shader Compile." << " ID: " << id << ", log: " << infoLog << std::endl;
        }
    }
    else
    {
        glGetShaderiv(id, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(id, 512, nullptr, infoLog);
            std::cerr << "Failed to Program Compile." << " ID: " << id << ", log: " << infoLog << std::endl;
        }
    }
}

int main()
{
    if(!glfwInit())
    {
        std::cerr << "Failed to glfw initalization." << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(ErrorCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    if(!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress)))
    {
        std::cerr << "Failed to glad load." << std::endl;
        return -1;
    }
    Mesh mesh(cubeVertices, cubeIndices);
    Shader shader("../src/Shader/vertexShader.vert", "../src/Shader/framentShader.frag");
    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.5,0.5,0.5,1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.UseProgram();
        mesh.DrawMesh();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void ErrorCallback(int error_code, const char* description)
{
    std::cerr << "error code: "<< error_code << ", description: " << description << std::endl;
}

void WindowSizeCallback(GLFWwindow* window, int width, int heigth)
{
    WIDTH = width;
    HEIGHT = heigth;
}

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}