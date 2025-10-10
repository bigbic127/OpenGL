#include "app.hpp"
#include "window.hpp"
#include "scene/world.hpp"
#include "renderer/renderer.hpp"
#include "scene/actor.hpp"
#include "logger.hpp"
#include "system/folder.hpp"

std::vector<Vertex> cubeVertices = {
    // Front face
    {{-1.0f, -1.0f,  1.0f},{0.0f,  0.0f,  1.0f},{0.0f, 0.0f}}, // Bottom-left
    {{ 1.0f, -1.0f,  1.0f},{0.0f,  0.0f,  1.0f},{1.0f, 0.0f}}, // Bottom-right
    {{ 1.0f,  1.0f,  1.0f},{0.0f,  0.0f,  1.0f},{1.0f, 1.0f}}, // Top-right
    {{-1.0f,  1.0f,  1.0f},{0.0f,  0.0f,  1.0f},{0.0f, 1.0f}}, // Top-left
    // Back face
    {{-1.0f, -1.0f, -1.0f},{0.0f,  0.0f, -1.0f},{1.0f, 0.0f}},
    {{ 1.0f, -1.0f, -1.0f},{0.0f,  0.0f, -1.0f},{0.0f, 0.0f}},
    {{ 1.0f,  1.0f, -1.0f},{0.0f,  0.0f, -1.0f},{0.0f, 1.0f}},
    {{-1.0f,  1.0f, -1.0f},{0.0f,  0.0f, -1.0f},{1.0f, 1.0f}},
    // Left face
    {{-1.0f, -1.0f, -1.0f},{-1.0f,  0.0f,  0.0f},{0.0f, 0.0f}},
    {{-1.0f, -1.0f,  1.0f},{-1.0f,  0.0f,  0.0f},{1.0f, 0.0f}},
    {{-1.0f,  1.0f,  1.0f},{-1.0f,  0.0f,  0.0f},{1.0f, 1.0f}},
    {{-1.0f,  1.0f, -1.0f},{-1.0f,  0.0f,  0.0f},{0.0f, 1.0f}},
    // Right face
    {{ 1.0f, -1.0f, -1.0f},{1.0f,  0.0f,  0.0f},{1.0f, 0.0f}},
    {{ 1.0f, -1.0f,  1.0f},{1.0f,  0.0f,  0.0f},{0.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f},{1.0f,  0.0f,  0.0f},{0.0f, 1.0f}},
    {{ 1.0f,  1.0f, -1.0f},{1.0f,  0.0f,  0.0f},{1.0f, 1.0f}},
    // Top face
    {{-1.0f,  1.0f, -1.0f},{0.0f,  1.0f,  0.0f},{0.0f, 1.0f}},
    {{-1.0f,  1.0f,  1.0f},{0.0f,  1.0f,  0.0f},{0.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f},{0.0f,  1.0f,  0.0f},{1.0f, 0.0f}},
    {{ 1.0f,  1.0f, -1.0f},{0.0f,  1.0f,  0.0f},{1.0f, 1.0f}},
    // Bottom face
    {{-1.0f, -1.0f, -1.0f},{0.0f, -1.0f,  0.0f},{1.0f, 1.0f}},
    {{-1.0f, -1.0f,  1.0f},{0.0f, -1.0f,  0.0f},{1.0f, 0.0f}},
    {{ 1.0f, -1.0f,  1.0f},{0.0f, -1.0f,  0.0f},{0.0f, 0.0f}},
    {{ 1.0f, -1.0f, -1.0f},{0.0f, -1.0f,  0.0f},{0.0f, 1.0f}}
};
std::vector<unsigned int> cubeIndices = {
    // Front face (CCW)
     2, 3, 0,  0, 1, 2,
    // Back face (CCW)
     6, 5, 4,  4, 7, 6,
    // Left face (CCW)
     10,11,8, 8, 9, 10,
    // Right face (CCW)
    14,13,12, 12,15,14,
    // Top face
    18,19,16, 16,17,18,
    // Bottom face
    20,23,22, 22,21,20
};

int main()
{
    Window window;
    World world;
    std::unique_ptr<IRenderer> renderer;
    renderer = std::make_unique<OpenGLRenderer>();
    if(window.Init())
    {
        //source
        std::shared_ptr<IMesh> cubeMesh =std::make_shared<Mesh>(cubeVertices, cubeIndices);
        std::string vertexShaderPath = "/shader/standard.vert";
        std::string fragShaderPath = "/shader/standard.frag";
        std::string diffuseTexturePath = "/assets/container2.png";
        std::shared_ptr<Shader> shader = std::make_shared<Shader>(vertexShaderPath, fragShaderPath);
        std::shared_ptr<Material> material = std::make_shared<Material>(shader);
        std::shared_ptr<Texture> texture = std::make_shared<Texture>(GetFullPath(diffuseTexturePath), 0);
        material->AddTexture(texture, "diffuseTexture");
        std::shared_ptr<Material> lightMaterial = std::make_shared<Material>(shader);
        //material parameter
        material->SetDiffuseTexture(true);
        lightMaterial->SetBaseColor(glm::vec3(1.0f, 0.0f, 0.0f));
        lightMaterial->SetAmbientColor(glm::vec3(1.0f, 1.0f, 1.0f));
        //camera
        auto cameraActor = world.CreateActor();
        cameraActor->name = "Camera";
        cameraActor->AddComponent<CameraComponent>();
        CameraComponent* cameraComponent = cameraActor->GetComponent<CameraComponent>();
        cameraActor->GetComponent<CameraComponent>()->SetPosition(glm::vec3(0.0f, 2.0f, -10.0f));
        //light
        auto lightActor = world.CreateActor();
        lightActor->name = "Light";
        lightActor->AddComponent<MeshComponent>(cubeMesh);
        lightActor->GetComponent<MeshComponent>()->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
        lightActor->GetComponent<MeshComponent>()->SetPosition(glm::vec3(0.0f, 2.0f, 1.0f));
        lightActor->GetComponent<MeshComponent>()->SetMaterial(lightMaterial);
        lightActor->AddComponent<LightComponent>();
        lightActor->GetComponent<LightComponent>()->SetPosition(glm::vec3(0.0f, 2.0f, 1.0f));
        LightComponent* lightComponent = lightActor->GetComponent<LightComponent>();
        //mesh
        auto actor = world.CreateActor();
        actor->name = "CubeMeshComponent";
        actor->AddComponent<MeshComponent>(cubeMesh);
        actor->GetComponent<MeshComponent>()->SetMaterial(material);
        //World Setting
        world.SetCurrentCamera(cameraComponent);
        world.AddLight(lightComponent);

        lightComponent->SetIntensity(5.0f);
        lightComponent->SetColor(glm::vec3(1.0f,0.0f,0.0f));


        renderer->Init();
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