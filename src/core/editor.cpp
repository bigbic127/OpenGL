#include "editor.hpp"
#include "window.hpp"
#include "resource/resource.hpp"
#include "input.hpp"
#include "renderer/renderer.hpp"
#include "system/folder.hpp"
#include "logger.hpp"
#include <stb_image.h>

Editor::~Editor()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    glDeleteTextures(1, &openFolderID);
    ImGui::DestroyContext();
}

void Editor::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard |
                     ImGuiConfigFlags_DockingEnable |
                     ImGuiConfigFlags_ViewportsEnable;
    CreateStyle();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    //font
    const std::string fontPath = GetFullPath("/fonts/NanumSquareB.ttf");
    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
    if(font==NULL)
    {
        Logger::ErrorMessage("Coluld Not load font file.");
        io.Fonts->AddFontDefault();
    }
    else
        io.FontDefault = font;
    CreateIcon();
}

GLuint Editor::LoadIconTexture(const std::string path)
{
    int w, h, c;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 0);
    if(!data)
    {
        Logger::ErrorMessage("Faield to Image load.");
        return 0;
    }
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return id;    
}

void Editor::CreateIcon()
{
    openFolderID = LoadIconTexture(GetFullPath("/icons/open-folder.png"));
}

void Editor::Update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    CreateLayout();
    //ImGui::ShowDemoWindow();
    {
        ImGuiIO& io = ImGui::GetIO();
        if(ImGui::IsKeyPressed(ImGuiKey_Delete))
        {
            
            if(selectedNode != nullptr)
            {
                if(auto ptr = world.lock())
                {
                    std::cerr<<selectedNode->actor->name<<std::endl;

                    ptr->DeleteActor(selectedNode->actor);
                }
                nodes.erase(nodes.begin() + selectedNode->id);
                selectedNode = nullptr;

            }
        }
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(window);
    }
}

void Editor::CreateLayout()
{
    //Create MenuBar
    if(ImGui::BeginMainMenuBar())
    {
        ImGui::Separator();
        ImGui::SameLine();
        if(ImGui::BeginMenu("파일"))
        {
            if(ImGui::MenuItem("불러오기", "Ctrl + O"))
            {
                //Action
                resourceManager->LoadModel();
            }
            ImGui::Separator();
            if(ImGui::MenuItem("종료", "Alt + F4"))
            {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::EndMenu();
        }
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        if(ImGui::BeginMenu("추가"))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 0.5f));
            ImGui::Text("Standard Mesh");
            ImGui::PopStyleColor();
            ImGui::Separator();
            if(ImGui::MenuItem("Cube Mesh"))
            {
                resourceManager->CreateStandardShpae(ShapeType::CUBE);
            }
            if(ImGui::MenuItem("Sphere Mesh"))
            {
                resourceManager->CreateStandardShpae(ShapeType::SPHERE);
            }
            if(ImGui::MenuItem("Cylinder Mesh"))
            {
                //
            }
            if(ImGui::MenuItem("Cone Mesh"))
            {
                //
            }
            if(ImGui::MenuItem("Plan Mesh"))
            {
                //
            }
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 0.5f));
            ImGui::Text("Light");
            ImGui::PopStyleColor();
            ImGui::Separator();
            if(ImGui::MenuItem("Directional"))
            {
                //
            }
            if(ImGui::MenuItem("Spot"))
            {
                //
            }
            if(ImGui::MenuItem("Point"))
            {
                //
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Camera"))
            {
                //
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    //Create ToolBar
    ImGuiViewport* guiViewport = ImGui::GetMainViewport();
    float toolbarHeight = ImGui::GetWindowHeight();
    ImGui::SetNextWindowPos(guiViewport->WorkPos);//위치
    ImGui::SetNextWindowSize(ImVec2(guiViewport->WorkSize.x, 0.0f));//사이즈
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));//스타일설정
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_AlwaysAutoResize;
    if(ImGui::Begin("mainToolbar", nullptr, flags))
    {
        ImGui::Separator();
        ImGui::SameLine();
        if(ImGui::ImageButton("maintoolBarOpenButton", (void*)(intptr_t)openFolderID, ImVec2(32,32)))
        {
            resourceManager->LoadModel();
        }
        if(ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("파일 불러오기 Ctrl + O");
            ImGui::EndTooltip();
        }
        toolbarHeight = ImGui::GetWindowHeight();
        ImGui::Separator();
        ImGui::End();
    }
    ImGui::PopStyleVar();//스타일해제
    //Create DockSpace
    guiViewport = ImGui::GetMainViewport();
    ImVec2 dockPos = guiViewport->WorkPos;
    ImVec2 dockSize = guiViewport->WorkSize;
    dockPos.y += toolbarHeight;
    dockSize.y -= toolbarHeight;
    ImGui::SetNextWindowPos(dockPos);
    ImGui::SetNextWindowSize(dockSize);
    ImGui::SetNextWindowViewport(guiViewport->ID);
    flags = ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBringToFrontOnFocus | //포커스 시 앞으로 나오지 않게
            ImGuiWindowFlags_NoNavFocus;             //네비게이션 포커스 받지 않게
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f,0.0f));
    ImGui::Begin("dockSpace", nullptr, flags);
    ImGui::PopStyleVar();
    ImGuiID dockSpaceID = ImGui::GetID("dockSpace");
    ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();
    //Create Hierarchy
    if(ImGui::Begin("아웃라이너"))
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_SpanAvailWidth |
                                   ImGuiTreeNodeFlags_Leaf;
                                   //ImGuiTreeNodeFlags_Selected;
        if (ImGui::TreeNodeEx("Scene", flags))
        {
            ImGui::Indent(10.0f);
            int selectedIndex = -1;
            for(size_t i=0;i<nodes.size(); i++)
            {
                bool selected = (selectedIndex == i);
                if(ImGui::Selectable(nodes[i]->name.c_str(), selected))
                {
                    Logger::ErrorMessage(nodes[i]->name.c_str());
                    selectedNode = nodes[i].get();
                    selectedNode->actor->SetID(i);
                    selectedNode->id = i;
                    transform = selectedNode->actor->GetComponent<SceneComponent>()->GetTransform();
                }
            }   
            ImGui::TreePop();
        }
        ImGui::End();
    }
    //Create Scene
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if(ImGui::Begin("scene"))
    {
        ImVec2 sceneSize = ImGui::GetContentRegionAvail();
        if(sceneSize.x > 0 && sceneSize.y >0)
            renderer->ResizeBuffer((int)sceneSize.x, (int)sceneSize.y);
        ImGui::Image((void*)(intptr_t)renderer->GetFrameBuffer(), sceneSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
    ImGui::PopStyleVar();
    //Create Detail
    if(ImGui::Begin("디테일"))
    {
        if (ImGui::BeginTabBar("MyTabBar"))
        {
            if (ImGui::BeginTabItem("Transform"))
            {
                std::string name = "";
                if(selectedNode != nullptr)
                    name = selectedNode->name;
                ImGui::Text("Mesh Name: ");
                ImGui::SameLine();
                ImGui::Text(name.c_str());
                ImGui::Separator();
                if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat3("Position", &transform.position.x, 0.1f);
                    ImGui::DragFloat3("Rotation", &transform.rotation.x, 0.1f);
                    ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
                    if(selectedNode != nullptr)
                    {
                        selectedNode->actor->GetComponent<SceneComponent>()->SetTransform(transform);
                        if(selectedNode->actor->GetComponent<LightComponent>() !=nullptr)
                            selectedNode->actor->GetComponent<LightComponent>()->SetTransform(transform);
                    }
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        
        ImGui::End();
    }
}

void Editor::CreateStyle()
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.5f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
}

Node* Editor::CreateNode(Actor* actor)
{  
    auto node = std::make_unique<Node>();
    Node* ptr = node.get();
    node->actor = actor;
    node->name = actor->name;
    nodes.push_back(std::move(node));
    return ptr;
}
