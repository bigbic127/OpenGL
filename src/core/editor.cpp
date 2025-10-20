#include "editor.hpp"
#include "window.hpp"

Editor::~Editor()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Editor::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        CreateStyle();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGui::StyleColorsDark();
}

void Editor::Update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    CreateLayout();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::CreateLayout()
{
    TopMenuBar();
}

void Editor::TopMenuBar()
{
    ImVec2 size = ImGui::GetIO().DisplaySize;
    float height = 60.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(size.x, height));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(45, 45, 48, 255));

    ImGui::Begin("TopBar", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings
    );

    ImGui::SameLine();
    if (ImGui::Button("File")) {}
    ImGui::SameLine();
    if (ImGui::Button("Edit")) {}
    ImGui::SameLine();
    if (ImGui::Button("View")) {}

    ImGui::SameLine(size.x / 2 - 40);
    ImGui::Text("Mate CMF");

    ImGui::SameLine(size.x - 90);
    ImGui::Button("_"); ImGui::SameLine();
    ImGui::Button("_"); ImGui::SameLine();
    if (ImGui::Button("x")) exit(0);

    ImGui::End();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void Editor::CreateStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.5f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
}