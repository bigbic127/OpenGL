#include "app.hpp"
#include "window.hpp"
#include "editor.hpp"
#include "scene/world.hpp"
#include "renderer/renderer.hpp"
#include "scene/actor.hpp"
#include "logger.hpp"
#include "system/folder.hpp"
#include "core/input.hpp"
#include "resource/resource.hpp"

int main()
{
    //create instance;
    Window window;
    std::shared_ptr<World> world = std::make_shared<World>();
    ResourceManager resourceManager(world);
    std::unique_ptr<IRenderer> renderer = std::make_unique<OpenGLRenderer>();//OpenGL, Vulkan Select
    if(window.Init())
    {
        Input input;
        input.SetResourceManager(&resourceManager);
        Editor editor(window.GetWindow());
        editor.Init();

        //resourceManager.LoadModel();
        //source_mesh
        std::shared_ptr<IMesh> cubeMesh =std::make_shared<CubeMesh>();
        std::shared_ptr<IMesh> sphereMesh = std::make_shared<ShpereMesh>(32,32,1.0f);
        std::shared_ptr<IMesh> coneMesh = std::make_shared<ConeMesh>(3,3.0f,1.0f);
        std::shared_ptr<IMesh> cylinderMesh = std::make_shared<CylinderMesh>(32, 2.0f, 2.0f);
        std::shared_ptr<IMesh> planeMesh = std::make_shared<PlaneMesh>(10.0f);
        //source_shader
        std::string vertexShaderPath = "/shader/standard.vert";
        std::string fragShaderPath = "/shader/standard.frag";
        std::string diffuseTexturePath = "/assets/container2.png";
        std::string specularTexturePath = "/assets/container2_specular.png";
        std::string moonTexturePath = "/assets/mercury.jpg";
        std::string cocacolaTexturePath = "/assets/cocacola.jpg";
        std::shared_ptr<Shader> shader = std::make_shared<Shader>(vertexShaderPath, fragShaderPath);
        //standard material
        std::shared_ptr<Material> standardMaterial = std::make_shared<Material>(shader);
        //cube material
        std::shared_ptr<Material> boxMaterial = std::make_shared<Material>(shader);
        std::shared_ptr<Texture> boxDiffTexture = std::make_shared<Texture>(GetFullPath(diffuseTexturePath));
        std::shared_ptr<Texture> boxSpecTexture = std::make_shared<Texture>(GetFullPath(specularTexturePath));
        boxMaterial->AddTexture(boxDiffTexture, "diffuseTexture");
        boxMaterial->AddTexture(boxSpecTexture, "specularTexture");
        //moon material
        std::shared_ptr<Material> moonMaterial = std::make_shared<Material>(shader);
        std::shared_ptr<Texture> moonTexture = std::make_shared<Texture>(GetFullPath(moonTexturePath));
        moonMaterial->AddTexture(moonTexture, "diffuseTexture");
        //cocacola material
        std::shared_ptr<Material> cocacolaMaterial = std::make_shared<Material>(shader);
        std::shared_ptr<Texture> cocacolaTexture = std::make_shared<Texture>(GetFullPath(cocacolaTexturePath));
        cocacolaMaterial->AddTexture(cocacolaTexture, "diffuseTexture");
        //light material
        std::shared_ptr<Material> lightMaterial = std::make_shared<Material>(shader);
        //material parameter
        standardMaterial->SetBaseColor(glm::vec3(0.25f));
        standardMaterial->SetAmbientColor(glm::vec3(0.1f));
        boxMaterial->SetAmbientColor(glm::vec3(0.5f));
        moonMaterial->SetBaseColor(glm::vec3(0.3f));
        cocacolaMaterial->SetBaseColor(glm::vec3(0.5f));
        cocacolaMaterial->SetAmbientColor(glm::vec3(0.7f));
        lightMaterial->SetBaseColor(glm::vec3(1.0f));
        lightMaterial->SetAmbientColor(glm::vec3( 1.0f));
        //camera
        auto cameraActor = world->CreateActor();
        cameraActor->name = "Camera";
        cameraActor->AddComponent<CameraComponent>();
        CameraComponent* cameraComponent = cameraActor->GetComponent<CameraComponent>();
        cameraActor->GetComponent<CameraComponent>()->SetPosition(glm::vec3(0.0f, 2.0f, -10.0f));
        //cameraActor->GetComponent<CameraComponent>()->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));

        //light
        auto lightActor = world->CreateActor();
        lightActor->name = "Light";
        glm::vec3 pos = {0.0f, 3.0f, 1.0f};
        glm::vec3 rot = {0.0f, 0.0f, 45.0f};
        lightActor->AddComponent<MeshComponent>(coneMesh);
        lightActor->GetComponent<MeshComponent>()->SetScale(glm::vec3(0.05f, -0.5f, 0.05f));
        lightActor->GetComponent<MeshComponent>()->SetPosition(pos);
        lightActor->GetComponent<MeshComponent>()->SetRotation(rot);
        lightActor->GetComponent<MeshComponent>()->SetMaterial(lightMaterial);
        lightActor->AddComponent<LightComponent>();
        lightActor->GetComponent<LightComponent>()->SetPosition(pos);
        lightActor->GetComponent<LightComponent>()->SetRotation(rot);
        LightComponent* lightComponent = lightActor->GetComponent<LightComponent>();
        //mesh
        auto actor = world->CreateActor();
        actor->name = "CubeMesh01";
        actor->AddComponent<MeshComponent>(cubeMesh);
        actor->GetComponent<MeshComponent>()->SetMaterial(boxMaterial);
        //actor->GetComponent<MeshComponent>()->SetRotation(glm::vec3(0.0f, -25.0f, 0.0f));
        //mesh02
        auto actor2 = world->CreateActor();
        actor2->name = "CylinderMesh01";
        actor2->AddComponent<MeshComponent>(cylinderMesh);
        actor2->GetComponent<MeshComponent>()->SetMaterial(cocacolaMaterial);  
        actor2->GetComponent<MeshComponent>()->SetPosition(glm::vec3(3.0f,0.0f,-1.5f)); 
        actor2->GetComponent<MeshComponent>()->SetScale(glm::vec3(0.1f, 0.5f, 0.1f));
        //actor->GetComponent<MeshComponent>()->SetRotation(glm::vec3(0.0f, 45.0f, 0.0f));
        //mesh03
        auto actor3 = world->CreateActor();
        actor3->name = "SphereMesh01";
        actor3->AddComponent<MeshComponent>(sphereMesh);
        actor3->GetComponent<MeshComponent>()->SetMaterial(moonMaterial);  
        actor3->GetComponent<MeshComponent>()->SetPosition(glm::vec3(-3.0f,0.0f,-1.5f)); 
        //mesh04
        auto actor4 = world->CreateActor();
        actor4->name = "PlaneMesh01";
        actor4->AddComponent<MeshComponent>(planeMesh);
        actor4->GetComponent<MeshComponent>()->SetMaterial(standardMaterial);  
        actor4->GetComponent<MeshComponent>()->SetPosition(glm::vec3(0.0f,-1.0f,0.0f)); 

        //World Setting
        world->SetCurrentCamera(cameraComponent);
        world->AddLight(lightComponent);

        lightComponent->SetIntensity(5.0f);
        lightComponent->SetColor(glm::vec3(1.0f,1.0f,1.0f));

        //Initialization
        input.SetCameraComponent(world->GetCurrentCamera());
        window.SetInput(&input);
        renderer->Init();

        //MainLoop
        while(!window.ShouldClose())
        {
            float deltaTime = world->GetDeltaTime();
            cameraActor->GetComponent<CameraComponent>()->SetAspect(window.GetAspect());
            glm::vec3 rot = actor->GetComponent<MeshComponent>()->GetRotation();
            rot.y += 10.0f * deltaTime;
            actor->GetComponent<MeshComponent>()->SetRotation(rot);
            rot = actor2->GetComponent<MeshComponent>()->GetRotation();
            rot.y -= 10.0f * deltaTime;
            actor2->GetComponent<MeshComponent>()->SetRotation(rot);
            rot = actor3->GetComponent<MeshComponent>()->GetRotation();
            rot.y += 50.0f * deltaTime;
            actor3->GetComponent<MeshComponent>()->SetRotation(rot);
            
            //inputLoop
            input.Process(window.GetWindow(), deltaTime);
            
            //UpdataLoop
            world->Update();
            //RenderLoop
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