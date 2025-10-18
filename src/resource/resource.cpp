#include "resource.hpp"
#include "fileDialog.hpp"
#include "core/logger.hpp"
#include "renderer/mesh.hpp"
#include "renderer/material.hpp"

void ResourceManager::LoadModel()
{
    std::vector<std::filesystem::path> paths;
    paths = FileDialog::ShowFileDialog(false);

    Assimp::Importer importer;
    for(auto& path:paths)
    {
        const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate|aiProcess_FlipUVs);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Logger::ErrorMessage("Failed to file load.");
            return;
        }
        std::cerr << "sucesses file laod." << std::endl;
        std::cerr << "mesh count:" << scene->mNumMeshes<< std::endl;
        std::cerr << "material count:" << scene->mNumMaterials<< std::endl;
        std::cerr << "texture count:" << scene->mNumTextures<< std::endl;
        std::cerr << "animation count:" << scene->mNumAnimations<< std::endl;
        std::cerr << "skeletons count:" << scene->mNumSkeletons<< std::endl;
        std::cerr << "camera count:" << scene->mNumCameras<< std::endl;
        std::cerr << "light count:" << scene->mNumLights<< std::endl;
        ProcessMesh(scene);
        ProcessMaterial(scene);
        ProcessTexture(scene);
        //ProcessNode(scene->mRootNode, scene);
    }
}

void ResourceManager::ProcessMesh(const aiScene* scene)
{
    for(size_t i=0;i<scene->mNumMeshes;i++)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        aiMesh* mesh = scene->mMeshes[i];
        for(size_t j=0;j<mesh->mNumVertices;j++)
        {
            Vertex vertex;
            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;
            if(mesh->HasNormals())
            {
                vertex.normal.x = mesh->mNormals[i].x;
                vertex.normal.y = mesh->mNormals[i].y;
                vertex.normal.z = mesh->mNormals[i].z;
            }
            if(mesh->mTextureCoords[0])
            {
                vertex.texcoord.x = mesh->mTextureCoords[0][i].x;
                vertex.texcoord.y = mesh->mTextureCoords[0][i].y;
            }
            if(mesh->mTangents)
            {
                vertex.tangent.x =  mesh->mTangents[i].x;
                vertex.tangent.y = mesh->mTangents[i].y;
                vertex.tangent.z = mesh->mTangents[i].z;
            }
            if(mesh->mBitangents)
            {
                vertex.bitTangent.x = mesh->mBitangents[i].x;
                vertex.bitTangent.y = mesh->mBitangents[i].y;
                vertex.bitTangent.z = mesh->mBitangents[i].z;
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

void ResourceManager::ProcessMaterial(const aiScene* scene)
{
    for(size_t i=0;i<scene->mNumMaterials;i++)
    {
        MaterialParameter parameter;
        aiMaterial* material = scene->mMaterials[i];
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
        if(AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE,0, &texPath))
        {
            std::string path = texPath.C_Str();
            if(path[0] == '*')
            {
                int index = std::atoi(path.c_str()+1);
                const aiTexture* texture = scene->mTextures[index];
                if(texture)
                {
                    if(texture->mHeight == 0)
                    {
                        std::cerr << "texture (png,jpg)) size: " << texture->mWidth << ", format: " << texture->achFormatHint << std::endl;
                    }
                    else
                    {
                        std::cerr << "texture size: " << texture->mWidth << ", x: " << texture->mHeight << std::endl;
                    }
                }
            }
            else
            {
                std::cerr << "texture path: " << path << std::endl;
            }
        }
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
                unsigned char* data = reinterpret_cast<unsigned char*>(texture->pcData);
                std::shared_ptr<Texture> tex = std::make_shared<Texture>(data, texture->mWidth);
            }
        }
    }
}

void ResourceManager::ProcessNode(aiNode* node, const aiScene* scene)
{
    for(size_t i=0;i<node->mNumMeshes;i++)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        aiMesh* mesh = scene->mMeshes[i];
        for(size_t i=0;i<mesh->mNumVertices;i++)
        {
            Vertex vertex;
            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;
            if(mesh->HasNormals())
            {
                vertex.normal.x = mesh->mNormals[i].x;
                vertex.normal.y = mesh->mNormals[i].y;
                vertex.normal.z = mesh->mNormals[i].z;
            }
            if(mesh->mTextureCoords[0])
            {
                vertex.texcoord.x = mesh->mTextureCoords[0][i].x;
                vertex.texcoord.y = mesh->mTextureCoords[0][i].y;
                vertex.tangent.x =  mesh->mTangents[i].x;
                vertex.tangent.y = mesh->mTangents[i].y;
                vertex.tangent.z = mesh->mTangents[i].z;
                vertex.bitTangent.x = mesh->mBitangents[i].x;
                vertex.bitTangent.y = mesh->mBitangents[i].y;
                vertex.bitTangent.z = mesh->mBitangents[i].z;
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
        unsigned int materialIndex = mesh->mMaterialIndex;
        aiMaterial* material = scene->mMaterials[materialIndex];

        //Create Actor
        if (auto ptr = world.lock())
        {
            std::shared_ptr<IMesh> mesh = std::make_shared<Mesh>(vertices, indices);

            meshes.push_back(mesh);
            auto actor = ptr->CreateActor();
            actor->AddComponent<MeshComponent>(mesh);
        }
    }
    for(size_t i=0;i<node->mNumChildren;i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}