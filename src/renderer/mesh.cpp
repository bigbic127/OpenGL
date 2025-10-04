#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex>& v, std::vector<unsigned int>& i):vertices(v), indices(i)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);//position
    glEnableVertexAttribArray(1);//normal
    glEnableVertexAttribArray(2);//texcoord
    glEnableVertexAttribArray(3);//tangent
    glEnableVertexAttribArray(4);//bitangent
    glEnableVertexAttribArray(5);//joints
    glEnableVertexAttribArray(6);//weights
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitTangent));
    glVertexAttribPointer(5, 4, GL_INT,   GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, joints));
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::Bind()
{
    glBindVertexArray(vao);
}

void Mesh::Unbind()
{
    glBindVertexArray(0);
}

void Mesh::Draw()
{
    glDrawElements(GL_TRIANGLES, indices.size()*sizeof(unsigned int), GL_UNSIGNED_INT, 0);
}