#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
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
    uniform mat4 transform;
    out vec4 outColor;
    void main()
    {
        gl_Position =  transform * vec4(vPos.xyz, 1.0f);
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

const float cubeVertices[] =
{
    1,  1, -1,  // 0: 오른쪽 위 앞
    1, -1, -1,  // 1: 오른쪽 아래 앞
   -1, -1, -1,  // 2: 왼쪽 아래 앞
   -1,  1, -1,  // 3: 왼쪽 위 앞
    1,  1,  1,  // 4: 오른쪽 위 뒤
    1, -1,  1,  // 5: 오른쪽 아래 뒤
   -1, -1,  1,  // 6: 왼쪽 아래 뒤
   -1,  1,  1   // 7: 왼쪽 위 뒤
};

const int cubeIndices[] =
{
    // 앞면
    0, 1, 2,
    0, 2, 3,
    // 뒷면
    4, 7, 6,
    4, 6, 5,
    // 왼쪽면
    3, 2, 6,
    3, 6, 7,
    // 오른쪽면
    0, 4, 5,
    0, 5, 1,
    // 윗면
    0, 3, 7,
    0, 7, 4,
    // 아랫면
    1, 5, 6,
    1, 6, 2
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
    unsigned int VAO, VBO, EBO;
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while(!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        glClearColor(0.5f,0.5f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        unsigned int cID, transID;
        cID = glGetUniformLocation(shaderProgram, "uColor");
        float val = glm::abs(glm::sin(glfwGetTime()*0.5f));
        glUniform4f(cID, 0.0f, val, 0.0f, 1.0f);
        transID = glGetUniformLocation(shaderProgram, "transform");
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f,0.1f,0.0f));
        transform = glm::scale(transform, glm::vec3(0.5f,0.5f,0.5f));
        glm::mat4 camera = glm::lookAt(glm::vec3(0.0f,5.0f,-5.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 perspective = glm::perspective((float)70.f, (float)WIDTH/HEIGHT, (float)0.1f, (float)1000.0f);
        transform = perspective * camera * transform;
        glUniformMatrix4fv(transID, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glFinish();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteBuffers(1, &VBO);
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