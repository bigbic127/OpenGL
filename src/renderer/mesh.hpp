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
        virtual void Bind() = 0;
        virtual void Draw() = 0;
        virtual void UnBind() = 0;
};

class Mesh:public IMesh
{
    public:
        Mesh(std::vector<Vertex>& v, std::vector<unsigned int>& i);
        ~Mesh();
        
        void Bind() override;
        void Unbind() override;
        void Draw() override;
    private:
        unsigned int vao, vbo, ebo;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
};