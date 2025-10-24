#include "resource.hpp"
#include "fileDialog.hpp"
#include "core/logger.hpp"
#include "renderer/mesh.hpp"
#include "renderer/material.hpp"
#include <sstream>
#include <iomanip>

ResourceManager::ResourceManager(std::shared_ptr<World> world):world(world)
{
    processShader();
    CreateShape();
}

void ResourceManager::LoadModel()
{
    std::vector<std::filesystem::path> paths;
    paths = FileDialog::ShowFileDialog(false);

    Assimp::Importer importer;
    if (paths.size()<=0)
        return;
    meshes.clear();
    textures.clear();
    materials.clear();
    for(auto& path:paths)
    {
        const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate|aiProcess_GenNormals);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Logger::ErrorMessage("Failed to file load.");
            return;
        }
        std::cerr << "sucesses file laod." << std::endl;
        ProcessMesh(scene);
        ProcessTexture(scene);
        ProcessMaterial(scene);
        ProcessNode(scene->mRootNode, scene);
        std::cerr << "meshes size: " << meshes.size() << std::endl;
        std::cerr << "textures size: " << textures.size() << std::endl;
        std::cerr << "materials size: " << materials.size() << std::endl;

        
    }
}

void ResourceManager::processShader()
{
    std::string vertexShaderPath = "/shader/standard.vert";
    std::string fragShaderPath = "/shader/standard.frag";
    std::shared_ptr<Shader> shader = std::make_shared<Shader>(vertexShaderPath, fragShaderPath);
    shaders.push_back(shader);
}

void ResourceManager::ProcessMesh(const aiScene* scene)
{
    for(size_t i=0;i<scene->mNumMeshes;i++)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        aiMesh* mesh = scene->mMeshes[i];
        std::cerr << mesh->mName.C_Str() << std::endl;
        for(size_t j=0;j<mesh->mNumVertices;j++)
        {
            Vertex vertex;
            vertex.position.x = mesh->mVertices[j].x;
            vertex.position.y = mesh->mVertices[j].y;
            vertex.position.z = mesh->mVertices[j].z;
            if(mesh->HasNormals())
            {
                vertex.normal.x = mesh->mNormals[j].x;
                vertex.normal.y = mesh->mNormals[j].y;
                vertex.normal.z = mesh->mNormals[j].z;
            }
            
            if(mesh->mTextureCoords[0])
            {
                vertex.texcoord.x = mesh->mTextureCoords[0][j].x;
                vertex.texcoord.y = mesh->mTextureCoords[0][j].y;
            }
            if(mesh->mTangents)
            {
                vertex.tangent.x =  mesh->mTangents[j].x;
                vertex.tangent.y = mesh->mTangents[j].y;
                vertex.tangent.z = mesh->mTangents[j].z;
            }
            if(mesh->mBitangents)
            {
                vertex.bitTangent.x = mesh->mBitangents[j].x;
                vertex.bitTangent.y = mesh->mBitangents[j].y;
                vertex.bitTangent.z = mesh->mBitangents[j].z;
            }
            vertices.push_back(vertex);
        }
        for(size_t i=0;i<mesh->mNumFaces;i++)
        {
            aiFace face = mesh->mFaces[i];
            for(size_t j=0;j<face.mNumIndices;j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
        std::shared_ptr<IMesh> iMesh = std::make_shared<Mesh>(vertices, indices);
        meshes.push_back(iMesh);
    }
}

void ResourceManager::ProcessTexture(const aiScene* scene)
{
    for(size_t i=0;i<scene->mNumTextures;i++)
    {
        const aiTexture* texture = scene->mTextures[i];
        if(texture)
        {
            if(texture->mHeight == 0)
            {
                std::cerr << texture->mFilename.C_Str() << std::endl;
                const unsigned char* data = reinterpret_cast<const unsigned char*>(texture->pcData);
                std::shared_ptr<Texture> tex = std::make_shared<Texture>(data, texture->mWidth);
                textures.push_back(tex);
            }
        }
    }
}

void ResourceManager::ProcessMaterial(const aiScene* scene)
{
    for(size_t i=0;i<scene->mNumMaterials;i++)
    {
        MaterialParameter parameter;
        aiMaterial* material = scene->mMaterials[i];
        std::cerr << material->GetName().C_Str() << std::endl;
        aiColor3D color(0.f,0.f,0.f);
        float value =0.f;
        aiString texPath;
        if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
        {
            parameter.baseColor.r = color.r;
            parameter.baseColor.g = color.g;
            parameter.baseColor.b = color.b;
        }
        if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
        {
            parameter.specularColor.r = color.r;
            parameter.specularColor.g = color.g;
            parameter.specularColor.b = color.b;
        }
        if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color))
        {
            parameter.ambientColor.r = color.r;
            parameter.ambientColor.g = color.g;
            parameter.ambientColor.b = color.b;
        }
        if(AI_SUCCESS == material->Get(AI_MATKEY_METALLIC_FACTOR, value))
        {
            parameter.metallicFactor = value;
        }
        if(AI_SUCCESS == material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value))
        {
            parameter.roughnessRactor = value;
        }
        if(AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, value))
        {
            parameter.shininess = value;
        }
        if(AI_SUCCESS == material->Get(AI_MATKEY_OPACITY, value))
        {
            parameter.opacity = value;
        }
        if(shaders.size()<0)
            return;
        std::shared_ptr<Material> mat = std::make_shared<Material>(shaders[0]);
        mat->SetParameter(parameter);
        if(AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE,0, &texPath))
        {
            std::string path = texPath.C_Str();
            if(path[0] == '*')
            {
                int index = std::atoi(path.c_str()+1);
                mat->AddTexture(textures[index], "diffuseTexture");
            }
            else
            {
                std::cerr << "texture path: " << path << std::endl;
            }
        }
        if(AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR,0, &texPath))
        {
            std::string path = texPath.C_Str();
            if(path[0] == '*')
            {
                int index = std::atoi(path.c_str()+1);
                mat->AddTexture(textures[index], "specularTexture");
            }
            else
            {
                std::cerr << "texture path: " << path << std::endl;
            }
        }
        materials.push_back(mat);
    }
}

void ResourceManager::ProcessNode(aiNode* node, const aiScene* scene)
{
    
    for(size_t i=0;i<node->mNumMeshes;i++)
    {
        //Create Actor
        unsigned int index = node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[index];
        unsigned int matIndex = mesh->mMaterialIndex;
        ;
        if (auto ptr = world.lock())
        {
            aiMatrix4x4 aiTransform = node->mTransformation;
            aiVector3D pos,scale;
            aiQuaternion rot;
            aiTransform.Decompose(scale, rot, pos);
            Transform transform;
            glm::quat quat = glm::quat(rot.w, rot.x, rot.y, rot.z);
            transform.position = glm::vec3(pos.x, pos.y, pos.z);
            transform.rotation = glm::degrees(glm::eulerAngles(glm::normalize(quat)));
            transform.scale = glm::vec3(scale.x, scale.y, scale.z);

            auto actor = ptr->CreateActor(mesh->mName.C_Str());
            actor->name = mesh->mName.C_Str();
            actor->AddComponent<MeshComponent>(meshes[index]);
            actor->GetComponent<MeshComponent>()->SetTransform(transform);
            actor->GetComponent<MeshComponent>()->SetMaterial(materials[matIndex]);
            materials[matIndex]->SetAmbientColor(glm::vec3(0.25f));
        }
    }
    for(size_t i=0;i<node->mNumChildren;i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void ResourceManager::CreateShape()
{
    std::shared_ptr<IMesh> cubeMesh =std::make_shared<CubeMesh>();
    std::shared_ptr<IMesh> sphereMesh = std::make_shared<ShpereMesh>(32,32,1.0f);
    std::shared_ptr<IMesh> coneMesh = std::make_shared<ConeMesh>(3,3.0f,1.0f);
    std::shared_ptr<IMesh> cylinderMesh = std::make_shared<CylinderMesh>(32, 2.0f, 2.0f);
    std::shared_ptr<IMesh> planeMesh = std::make_shared<PlaneMesh>(10.0f);
    standardShape.push_back(cubeMesh);
    standardShape.push_back(sphereMesh);
    standardShape.push_back(coneMesh);
    standardShape.push_back(cylinderMesh);
    standardShape.push_back(planeMesh);

    std::shared_ptr<Material> standardMaterial = std::make_shared<Material>(shaders[0]);
    standardMaterial->SetBaseColor(glm::vec3(0.5f));
    standardMaterial->SetAmbientColor(glm::vec3(0.1f));
    standardMaterials.push_back(standardMaterial);
}

void ResourceManager::CreateStandardShpae(ShapeType type)
{
    if(auto ptr = world.lock())
    {
        switch (type)
        {
        case ShapeType::CUBE:
            {
                int num = 0;
                for(auto& a:ptr->GetActors())
                {
                    if(a->name.compare(0,4,"Cube") == 0)
                    {
                        num +=1;
                    }
                }
                std::string name = "Cube";
                std::ostringstream oss;
                if(num >0)
                {
                    oss<<name<<std::setw(3)<<std::setfill('0')<<num;
                    name = oss.str();
                }
                auto actor = ptr->CreateActor(name);
                actor->name = name;
                actor->AddComponent<MeshComponent>(standardShape[0]);
                actor->GetComponent<MeshComponent>()->SetMaterial(standardMaterials[0]);
            }
            break;
        case ShapeType::SPHERE:
            {
                int num = 0;
                for(auto& a:ptr->GetActors())
                {
                    if(a->name.compare(0,6,"Sphere") == 0)
                    {
                        num +=1;
                    }
                }
                std::string name = "Sphere";
                std::ostringstream oss;
                if(num >0)
                {
                    oss<<name<<std::setw(3)<<std::setfill('0')<<num;
                    name = oss.str();
                }
                auto actor = ptr->CreateActor(name);
                actor->name = name;
                actor->AddComponent<MeshComponent>(standardShape[1]);
                actor->GetComponent<MeshComponent>()->SetMaterial(standardMaterials[0]);
            }
            break;
        default:
            break;
        }        
    }

}