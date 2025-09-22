#pragma once
#include "glad/glad.h"
#include <glm.hpp>
#include <vector>
#define MAX_BONE_INFLUENCE 4

struct surface
{
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    int boneIDs[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];
};

class Mesh
{
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
        ~Mesh();
        void SetupMesh();
        void DrawMesh();
    private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int vao, vbo, ebo;
};

