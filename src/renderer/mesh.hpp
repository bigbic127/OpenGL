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
        virtual void Unbind() = 0;
};

class Mesh:public IMesh
{
    public:
        Mesh(std::vector<Vertex>& v, std::vector<unsigned int>& i);
        ~Mesh();
        
        void Bind() override;
        void Unbind() override;
        void Draw() override;
    protected:
        unsigned int vao, vbo, ebo;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
};

class CubeMesh:public Mesh
{
    public:
        CubeMesh():Mesh(CreateVertices(),
                         CreateIndices()){}
        static std::vector<Vertex> CreateVertices()
        {
            std::vector<Vertex> vertices = {
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
            return vertices;
        }
        static std::vector<unsigned int> CreateIndices()
        {
            std::vector<unsigned int> indices = {
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
            return indices;
        }
};

class ShpereMesh:public Mesh
{
    public:
        ShpereMesh(int stacks, int slices, float radius):Mesh(CreateVertices(stacks, slices, radius),
                                                              CreateIndices(stacks, slices)){}
    private:
        static std::vector<Vertex> CreateVertices(int stacks, int slices, float radius)
        {
            std::vector<Vertex> vertices;
            constexpr float PI = 3.14159265359f;

            for (int lat = 0; lat <= stacks; ++lat)
            {
                float theta = lat * PI / stacks; // 위도 각도
                float sinTheta = sin(theta);
                float cosTheta = cos(theta);

                for (int lon = 0; lon <= slices; ++lon)
                {
                    float phi = lon * 2.0f * PI / slices; // 경도 각도
                    float sinPhi = sin(phi);
                    float cosPhi = cos(phi);

                    // ---- 위치 ----
                    glm::vec3 position(
                        radius * sinTheta * cosPhi,
                        radius * cosTheta,
                        radius * sinTheta * sinPhi
                    );

                    // ---- 법선 ---- (정규화된 방향 벡터)
                    glm::vec3 normal = glm::normalize(glm::vec3(
                        sinTheta * cosPhi,
                        cosTheta,
                        sinTheta * sinPhi
                    ));

                    // ---- 텍스처 좌표 ----
                    float u = static_cast<float>(lon) / slices;
                    float v = static_cast<float>(lat) / stacks;

                    Vertex vertex;
                    vertex.position = position;
                    vertex.normal = normal;
                    vertex.texcoord = glm::vec2(u, v);

                    vertices.push_back(vertex);
                }
            }
            return vertices;
        }

        static std::vector<unsigned int> CreateIndices(int stacks, int slices)
        {
            std::vector<unsigned int> indices;

            for (int lat = 0; lat < stacks; ++lat)
            {
                for (int lon = 0; lon < slices; ++lon)
                {
                    int current = lat * (slices + 1) + lon;
                    int next = current + slices + 1;
                    indices.push_back(current);
                    indices.push_back(current + 1);
                    indices.push_back(next);
                    indices.push_back(current + 1);
                    indices.push_back(next + 1);
                    indices.push_back(next);
                }
            }
            return indices;
        }
};

class ConeMesh:public Mesh
{
    public:
        ConeMesh(int slices, float radius, float height):Mesh(CreateVertices(slices, radius, height),
                                                              CreateIndices(slices)){}
    private:
        static std::vector<Vertex> CreateVertices(int slices, float radius, float height)
        {
            std::vector<Vertex> vertices;
            constexpr float PI = 3.14159265359f;
            float halfHeight = height * 0.5f;
            glm::vec3 apex(0.0f, halfHeight, 0.0f);
            glm::vec3 baseCenter(0.0f, -halfHeight, 0.0f);
            for (int i = 0; i <= slices; ++i)
            {
                float theta = (2.0f * PI * i) / slices;
                float x = radius * cos(theta);
                float z = radius * sin(theta);
                glm::vec3 pos(x, -halfHeight, z);
                glm::vec3 normal = glm::normalize(glm::vec3(x, radius / height, z));
                glm::vec2 uv(static_cast<float>(i) / slices, 1.0f);

                vertices.push_back({pos, normal, uv});
            }
            vertices.push_back({apex, glm::vec3(0, 1, 0), glm::vec2(0.5f, 0)});
            vertices.push_back({baseCenter, glm::vec3(0, -1, 0), glm::vec2(0.5f, 0.5f)});

            return vertices;        
        }
        static std::vector<unsigned int> CreateIndices(int slices)
        {
            std::vector<unsigned int> indices;

            int apexIndex = slices + 1;
            int baseCenterIndex = apexIndex + 1;
            for (int i = 0; i < slices; ++i)
            {
                int next = (i + 1) % slices;
                indices.push_back(apexIndex);
                indices.push_back(i);
                indices.push_back(next);
            }
            for (int i = 0; i < slices; ++i)
            {
                int next = (i + 1) % slices;
                indices.push_back(baseCenterIndex);
                indices.push_back(next);
                indices.push_back(i);
            }

            return indices;
        }
};

class CylinderMesh:public Mesh
{

};
