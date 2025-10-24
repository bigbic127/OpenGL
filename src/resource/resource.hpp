#pragma once
#include <vector>
#include <memory>
#include <renderer/mesh.hpp>
#include <renderer/shader.hpp>
#include <renderer/material.hpp>
#include <renderer/texture.hpp>
#include "scene/world.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

enum ShapeType
{
    CUBE = 0,
    SPHERE,
    CONE,
    CYLINDER,
    PLAN
};


class ResourceManager
{
    public:
        ResourceManager(std::shared_ptr<World> world);
        void LoadModel();
        void CreateStandardShpae(ShapeType type);

    private:
        void processShader();
        void ProcessMaterial(const aiScene* scene);
        void ProcessTexture(const aiScene* scene);
        void ProcessMesh(const aiScene* scene);
        void ProcessNode(aiNode* node, const aiScene* scene);
        void CreateShape();
        std::weak_ptr<World> world;
        std::vector<std::shared_ptr<IMesh>> standardShape;
        std::vector<std::shared_ptr<Material>> standardMaterials;
        std::vector<std::shared_ptr<IMesh>> meshes;
        std::vector<std::shared_ptr<Shader>> shaders;
        std::vector<std::shared_ptr<Material>> materials;
        std::vector<std::shared_ptr<Texture>> textures;
};