#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm.hpp>

const GLint WIDTH = 800;
const GLint HEIGHT = 600;

void Framebuffer_size_callback(GLFWwindow *window, int width, int height);

const GLchar* vertexShaderSource = R"(
    #version 450 core
    layout (location =0) in vec4 vPos;
    layout (location =1) in vec4 vColor;
    layout (location =2) in vec2 uvs;
    out vec4 outColor;
    void main()
    {
        gl_Position = vPos;
        outColor = vColor;
    }
)";

const GLchar* fragmentShaderSource = R"(
    #version 450 core
    out vec4 fragColor;
    in vec4 outColor;
    uniform vec4 uColor;
    void main()
    {
        fragColor = uColor;
    }
)";

const GLfloat vertices[] = 
{
    -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f,
    0.0f,  0.5f, 0.0f, 1.0f
};

const GLfloat vertices2[] = 
{
    0.5f,  1.0f, 0.0f, 1.0f,
    1.0f, -0.0f, 0.0f, 1.0f,
    -0.0f, -0.0f, 0.0f, 1.0f
};

const GLfloat colors[] =
{
    1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f,  0.0f, 1.0f, 1.0f 
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

    //Vertex Buffer
    unsigned int VAO, VBO[2];
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    while(!glfwWindowShouldClose(window))
    {
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

void Framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}