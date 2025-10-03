#pragma once
#include <glad/glad.h>
#include <vector>
#include <glm.hpp>

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec3 tangent;
    glm::vec3 bitTangent;
    int joints[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];
};

class IMesh
{
    public:
        virtual ~IMesh() = default;
        virtual void Draw(){};
};

class Mesh
{
    public:
        Mesh(std::vector<Vertex>vertices, std::vector<unsigned int> indices);
        ~Mesh();
        
        void Bind();
        void Unbind();
    private:
        unsigned int vao, vbo, ebo;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
};