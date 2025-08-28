#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

GLint WIDTH = 800;
GLint HEIGHT = 600;

void Window_size_callback(GLFWwindow* window, int width, int height);
void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);
void error_callback(int error, const char* description);

const GLchar* vertexShaderSource = R"(
    #version 410 core
    layout (location =0) in vec3 vPos;
    layout (location =1) in vec2 aTexCoord;
    uniform mat4 transform;
    out vec2 texCoord;
    void main()
    {
        gl_Position =  transform * vec4(vPos.xyz, 1.0f);
        texCoord = aTexCoord;
    }
)";

const GLchar* fragmentShaderSource = R"(
    #version 410 core
    out vec4 fragColor;
    in vec2 texCoord;
    uniform vec4 uColor;
    uniform sampler2D texture1;
    void main()
    {
        fragColor = texture(texture1, texCoord);
    }
)";

const float cubeVertices[] = {
    // positions        // UVs
     1,  1, -1,         1.0f, 1.0f, // 0
     1, -1, -1,         1.0f, 0.0f, // 1
    -1, -1, -1,         0.0f, 0.0f, // 2
    -1,  1, -1,         0.0f, 1.0f, // 3
     1,  1,  1,         1.0f, 1.0f, // 4
     1, -1,  1,         1.0f, 0.0f, // 5
    -1, -1,  1,         0.0f, 0.0f, // 6
    -1,  1,  1,         0.0f, 1.0f  // 7
};

const unsigned int cubeIndices[] = {
    // 앞
    0,1,2, 0,2,3,
    // 뒤
    4,7,6, 4,6,5,
    // 왼
    3,2,6, 3,6,7,
    // 오
    0,4,5, 0,5,1,
    // 위
    0,3,7, 0,7,4,
    // 아래
    1,5,6, 1,6,2
};

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
    glfwSetWindowSizeCallback(window, Window_size_callback);
    glfwSetFramebufferSizeCallback(window, Framebuffer_size_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        glfwDestroyWindow(window);
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
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
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //stb
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // y축 반전
    const char* path = "../resource/wall.jpg";
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else
        std::cerr<<"Failed to load texture\n";
    stbi_image_free(data);

    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    while(!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        glClearColor(0.5f,0.5f,0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        unsigned int cID, transID;
        cID = glGetUniformLocation(shaderProgram, "uColor");
        float val = glm::abs(glm::sin(glfwGetTime()*0.5f));
        glUniform4f(cID, 0.0f, val, 0.0f, 1.0f);
        // (model -> view -> perspective) -> (clip -> NDC)
        transID = glGetUniformLocation(shaderProgram, "transform");
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime() * 0.5f, glm::vec3(0.0f,0.1f,0.0f));
        transform = glm::scale(transform, glm::vec3(0.5f,0.5f,0.5f));
        glm::mat4 camera = glm::lookAt(glm::vec3(0.0f,0.5f,-2.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
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

void Window_size_callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
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