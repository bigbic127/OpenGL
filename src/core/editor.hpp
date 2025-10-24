#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <vector>
#include <memory>
#include "component/component.hpp"
#include "scene/world.hpp"

class Window;
class ResourceManager;
class Input;
class IRenderer;
class Actor;

enum NodeType
{
    Empty = 0,
    Mesh,
    Camera,
    Light    
};

struct Node
{
    std::string name;
    Actor* actor = nullptr;
    unsigned int id = -1;
};

class Editor
{
    public:
        Editor(GLFWwindow* w):window(w){}
        ~Editor();
        void Init();
        void Update();
        void CreateIcon();
        void CreateLayout();
        void CreateStyle();
        GLuint LoadIconTexture(const std::string path);
        void SetInput(Input* i){input = i;}
        void SetResourceManager(ResourceManager* r){resourceManager = r;}
        void SetRenderer(IRenderer* r){renderer = r;}
        void SetWorld(std::shared_ptr<World> w){world = w;}
        Node* CreateNode(Actor* actor);
        void DeleteNode();
    private:
        std::weak_ptr<World> world;
        Node* selectedNode = nullptr;
        Transform transform;
        GLFWwindow* window;
        ResourceManager* resourceManager;
        IRenderer* renderer;
        Input* input;
        GLuint openFolderID;
        std::vector<std::unique_ptr<Node>> nodes;
};