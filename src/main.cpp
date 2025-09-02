#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

int WIDTH = 800;
int HEIGHT = 600;

void Error_Callback(int error, const char* description);
void Error_ShaderCompile(int shaderID, std::string type);
void WindowResize_Callback(GLFWwindow* window, int width, int height);
void FrameBufferSize_Callback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);
void CursorPos_Callback(GLFWwindow* window, double xpos, double ypos);
void MouseButton_Callback(GLFWwindow* window, int button, int action, int mods);
void Scroll_Callback(GLFWwindow*window, double xoffset, double yoffset);

glm::vec3 LIGHT_POSTION = glm::vec3(2.0f, 2.0f, 0.5f);
glm::vec3 Camera_Position = glm::vec3(0.0f,0.0f, 3.0f);
glm::vec3 Camera_Front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Camera_Up = glm::vec3(0.0f, 1.0f, 0.0f);

float DeltaTime = 0.0f;

bool bMouseLeftClicked = false;
bool bFirstClicked = false;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float cubeVertices[] = {
    // Front face
    -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f, // Bottom-left
     1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f, // Bottom-right
     1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f, // Top-right
    -1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f, // Top-left

    // Back face
    -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,

    // Left face
    -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

    // Right face
     1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     1.0f,  1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,

    // Top face
    -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,

    // Bottom face
    -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
};

unsigned int cubeIndices[] = {
    // Front face (CCW)
     0, 3, 2,  2, 1, 0,
    // Back face (CCW)
     4, 5, 6,  6, 7, 4,
    // Left face (CCW)
     8,11,10, 10, 9, 8,
    // Right face (CCW)
    12,13,14, 14,15,12,
    // Top face
    16,19,18, 18,17,16,
    // Bottom face
    20,23,22, 22,21,20
};

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

const char* lightVertexShaderSource = R"(
    #version 410 core
    layout (location = 0) in vec3 vPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * model * vec4(vPos.xyz, 1.0f);
    }
)";

const char* lightFragmentShaderSource = R"(
    #version 410 core
    out vec4 FragColor;
    void main()
    {
        FragColor = vec4(1.0f);
    }
)";

const char* vertexShaderSource = R"(
    #version 410 core
    layout (location = 0) in vec3 vPos;
    layout (location = 1) in vec3 vNor;
    layout (location = 2) in vec2 vUVs;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    out vec2 TexCoord;
    out vec3 Normal;
    out vec3 Model;
    void main()
    {
        gl_Position = projection * view * model * vec4(vPos.xyz, 1.0f);
        Normal = mat3(transpose(inverse(model))) * vNor;
        TexCoord = vUVs;
        Model = vec3(model * vec4(vPos.xyz, 1.0f));

    }
)";

const GLchar* fragmentShaderSource = R"(
    #version 410 core
    out vec4 FragColor;
    in vec2 TexCoord;
    in vec3 Normal;
    in vec3 Model;
    uniform vec3 lightDirection = vec3(0.5f, -0.5f, 0.0f);//디렉셔널 라이트
    uniform vec3 lightPos;
    uniform vec3 viewPos;
    uniform vec3 lightColor;
    uniform vec3 objectColor = vec3(1.0, 1.0, 1.0);;
    uniform sampler2D texture1;
    uniform sampler2D texture2;

    float constant = 1.0f;
    float linear = 0.7f;
    float quadratic = 1.8f;

    float cutOff = 0.966;
    float outerCutOff= 0.953f;

    void main()
    {
        vec3 ambient = 0.4 * lightColor * objectColor * vec3(texture(texture1, TexCoord).rgb);
        vec3 normal = normalize(Normal);
        //diffuse
        vec3 lightDir = normalize(lightPos-Model);
        //lightDir = normalize(-lightDirection); //디렉셔널 라이트
        //lightDir = normalize(vec3(0,0,1) - Model);//스팟(카메라 기준)
        float lightValue  = max(dot(normal,lightDir), 0.0f);
        vec3 diffuse = 5.0 * lightValue * lightColor * objectColor * vec3(texture(texture1, TexCoord).rgb);
        //specular
        vec3 specDir = normalize(viewPos-Model);
        vec3 reflectDir = reflect(-lightDir, normal);
        float reflectValue = pow(max(dot(specDir, reflectDir), 0.0f), 32);
        vec3 specular = 20.0f * reflectValue * lightColor * objectColor * vec3(texture(texture2, TexCoord).rgb);
        //attenuation
        float distance = length(lightPos-Model);
        float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    
        //ambient *= attenuation;
        //diffuse *= attenuation;
        //specular *= attenuation;
        //spotlight (soft edges)
        float theta = dot(lightDir, normalize(-vec3(0,0,-1))); 
        float epsilon = (cutOff - outerCutOff);
        float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
        //diffuse  *= intensity;
        //specular *= intensity;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0f);
    }
)";

int main()
{
    if(!glfwInit())
    {
        std::cerr << "Failed to glfwInit()" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW Window : Opengl", nullptr, nullptr);
    if(!window)
    {
        std::cerr << "Failed to Create Window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr<< "Failed to GladLoader" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    glfwSetErrorCallback(Error_Callback);
    glfwSetWindowSizeCallback(window, WindowResize_Callback);
    glfwSetFramebufferSizeCallback(window, FrameBufferSize_Callback);
    glfwSetCursorPosCallback(window, CursorPos_Callback);
    glfwSetMouseButtonCallback(window, MouseButton_Callback);
    glfwSetScrollCallback(window, Scroll_Callback);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile("../resource/bunny.obj", aiProcess_Triangulate | aiProcess_FlipUVs);


    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*(sizeof(float)), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*(sizeof(float)), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*(sizeof(float)), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0); // VAO를 스테이트머신에서 초기화하여 추가작업은 VAO가 bind 되지 않는다.

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO); // lightVAO가 스테이트머신에 초기화, 추가작업은 lightVAO에 bind 된다.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*(sizeof(float)), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); // VAO 초기화
    glBindBuffer(GL_ARRAY_BUFFER, 0); // VBO 초기화
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // EBO 초기화

    unsigned int lightVertexShader, lightFragmentShader, lightShaderProgram;
    lightVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(lightVertexShader, 1, &lightVertexShaderSource, nullptr);
    glCompileShader(lightVertexShader);
    Error_ShaderCompile(lightVertexShader, "VERTEX");
    lightFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lightFragmentShader, 1, &lightFragmentShaderSource, nullptr);
    glCompileShader(lightFragmentShader);
    Error_ShaderCompile(lightFragmentShader, "FRAGMENT");
    lightShaderProgram = glCreateProgram();
    glAttachShader(lightShaderProgram, lightVertexShader);
    glAttachShader(lightShaderProgram, lightFragmentShader);
    glLinkProgram(lightShaderProgram);
    Error_ShaderCompile(lightShaderProgram, "PROGRAM");
    glUseProgram(lightShaderProgram);
    glDeleteShader(lightVertexShader);
    glDeleteShader(lightFragmentShader);

    unsigned int vertexShader, fragmentShader, shaderProgram;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    Error_ShaderCompile(vertexShader, "VERTEX");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    Error_ShaderCompile(fragmentShader, "FRAGMENT");
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    Error_ShaderCompile(shaderProgram, "PROGRAM");
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("../resource/container2.png", &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Failed to Image Load." << std::endl;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    unsigned int textureShaderID;
    textureShaderID = glGetUniformLocation(shaderProgram, "texture1");
    glUniform1i(textureShaderID, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int textureID2;
    glGenTextures(1, &textureID2);
    glBindTexture(GL_TEXTURE_2D, textureID2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("../resource/container2_specular.png", &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Failed to Image Load." << std::endl;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    textureShaderID = glGetUniformLocation(shaderProgram, "texture2");
    glUniform1i(textureShaderID, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 view(1.0f), projection(1.0f);
        view = glm::lookAt(Camera_Position, Camera_Position + Camera_Front, Camera_Up);
        projection = glm::perspective(glm::radians(70.0f), (float)WIDTH/HEIGHT, 0.1f, 1000.0f);

        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        for (size_t i = 0; i < 10; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, (cubePositions[i] * 0.5f));
            float angle = 20 * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, glm::vec3(0.4));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &LIGHT_POSTION[0]);
            glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 0.0f, 1.0f, -2.0f);
            //glDrawArrays(GL_TRIANGLES, 0, 3);
            glDrawElements(GL_TRIANGLES, sizeof(cubeIndices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        }
        glm::mat4 lightModel(1.0f);
        lightModel = glm::translate(lightModel, LIGHT_POSTION);
        lightModel = glm::scale(lightModel, glm::vec3(0.1f));
        glUseProgram(lightShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        //텍스쳐 bind
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureID2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(lightShaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void ProcessInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Camera_Position += cameraSpeed * Camera_Front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Camera_Position -= cameraSpeed * Camera_Front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Camera_Position -= glm::normalize(glm::cross(Camera_Front, Camera_Up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Camera_Position += glm::normalize(glm::cross(Camera_Front, Camera_Up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        Camera_Position += cameraSpeed * Camera_Up;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        Camera_Position -= cameraSpeed * Camera_Up;
}

void MouseButton_Callback(GLFWwindow* window, int button, int action, int mods)
{
    //std::cerr << "Button : " << button << " , Action : " << action <<" , Mods: "<<mods<< std::endl;
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
    std::cerr << "Button : " << bMouseLeftClicked << std::endl;
}

void Scroll_Callback(GLFWwindow*window, double xoffset, double yoffset)
{

}

void CursorPos_Callback(GLFWwindow* window, double xpos, double ypos)
{
    if(bMouseLeftClicked)
    {
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
        Camera_Front = glm::normalize(front);
        std::cerr << "CursorPos : " << offsetX << "," << offsetY << std::endl;
    }
}


void WindowResize_Callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
}


void FrameBufferSize_Callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Error_Callback(int error, const char* description)
{
    std::cerr <<"Error : " << error << " : " << description << std::endl;
}

void Error_ShaderCompile(int shaderID, std::string type)
{
    int success;
    char infoLog[512];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
            std::cerr << "Failed to " << type << " Compile : " << infoLog << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderID, 512, nullptr, infoLog);
            std::cerr << "Failed to " << type << " Compile : " << infoLog << std::endl;
        }
    }
}