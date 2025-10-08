#include "app.hpp"
#include "window.hpp"
#include "scene/world.hpp"
#include "renderer/renderer.hpp"
#include "scene/actor.hpp"
#include "logger.hpp"


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

int main()
{
    Window window;
    World world;
    std::unique_ptr<IRenderer> renderer;
    renderer = std::make_unique<OpenGLRenderer>();
    if(window.Init())
    {
        std::shared_ptr<IMesh> cubeMesh =std::make_shared<Mesh>(cubeVertices, cubeIndices);
        auto actor = world.CreateActor();
        actor->AddComponent<MeshComponent>(cubeMesh);
        actor->name = "CubeMeshComponent";
        std::string vertexShaderPath = "/shader/standard.vert";
        std::string fragShaderPath = "/shader/standard.frag";
        std::shared_ptr<Shader> shader = std::make_shared<Shader>(vertexShaderPath, fragShaderPath);
        std::shared_ptr<Material> material = std::make_shared<Material>(shader);
        actor->GetComponent<MeshComponent>()->SetMaterial(material);
        //actor->GetComponent<MeshComponent>()->SetScale(glm::vec3(0.25f,1.0f,0.5f));
        auto cameraActor = world.CreateActor();
        cameraActor->name = "Camera";
        cameraActor->AddComponent<CameraComponent>();
        CameraComponent* component = cameraActor->GetComponent<CameraComponent>();
        actor->GetComponent<MeshComponent>()->SetCameraComponent(component);
        cameraActor->GetComponent<CameraComponent>()->SetPosition(glm::vec3(0.0f, 1.0f, -5.0f));

        while(!window.ShouldClose())
        {
            cameraActor->GetComponent<CameraComponent>()->SetAspect(window.GetAspect());
            glm::vec3 rot = actor->GetComponent<MeshComponent>()->GetRotation();
            rot.y += 1;
            actor->GetComponent<MeshComponent>()->SetRotation(rot);
            
            world.Update();
            renderer->Clear();
            renderer->Begin();
            renderer->Render(world);
            renderer->End();
            window.SwapBuffer();
            window.PollEvent();
        }
    }
    return 0;
}