#pragma once
#include <vector>
#include <memory>
#include <renderer/mesh.hpp>
#include <renderer/material.hpp>
#include <renderer/texture.hpp>
#include "scene/world.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ResourceManager
{
    public:
        ResourceManager(std::shared_ptr<World> world):world(world){}
        void LoadModel();

    private:
        void ProcessMaterial(const aiScene* scene);
        void ProcessTexture(const aiScene* scene);
        void ProcessMesh(const aiScene* scene);
        void ProcessNode(aiNode* node, const aiScene* scene);


        std::weak_ptr<World> world;
        std::vector<std::shared_ptr<IMesh>> meshes;
        std::vector<Material> materials;
        std::vector<Texture> textures;
};