#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm.hpp>
#include <iostream>

const GLint WIDTH = 800;
const GLint HEIGHT = 600;

void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);
void error_callback(int error, const char* description);

const GLchar* vertexShaderSource = R"(
    #version 410 core
    layout (location =0) in vec3 vPos;
    layout (location =1) in vec3 vColor;
    layout (location =2) in vec2 uvs;
    out vec4 outColor;
    void main()
    {
        gl_Position = vec4(vPos.xyz, 1.0f);
        outColor = vec4(vColor.xyz, 1.0);
    }
)";

const GLchar* fragmentShaderSource = R"(
    #version 410 core
    out vec4 fragColor;
    in vec4 outColor;
    uniform vec4 uColor;
    void main()
    {
        fragColor = uColor;
    }
)";

class Mesh
{
};

const GLfloat vertices[] = 
{
    -0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
    0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,
};


void DrawTriangle()
{
    
}

int main()
{
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    glfwSetErrorCallback(error_callback);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        glfwDestroyWindow(window);
        std::cout << "faild window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, Framebuffer_size_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        glfwDestroyWindow(window);
        return -1;
    }
    //Shader
    unsigned int vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); 
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /*
    int success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "Failed to VertexShader Compile\n" << infoLog << endl; 
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "Failed to FragmentShader Compile\n" << infoLog << endl; 
    }
    */

    //Vertex Buffer
    unsigned int VAO, VBO[2];
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    while(!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        glClearColor(0.5f,0.5f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        unsigned int cID;
        cID = glGetUniformLocation(shaderProgram, "uColor");
        float val = glm::abs(glm::sin(glfwGetTime()*0.5f));
        glUniform4f(cID, 0.0f, val, 0.0f, 1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glFinish();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteBuffers(2, VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    glfwDestroyWindow(window);
    return 0;
}

void Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}