#pragma once
#include <glad/glad.h>
#include <string>
#include <memory>
#include "shader.hpp"

class Texture
{
    public:
        Texture(std::string path, unsigned int u);
        ~Texture();
        void Init(std::shared_ptr<Shader> shader, std::string& name);
        void Bind();
        void Unbind();
    private:
        unsigned int textureID, locationID, unitIndex;
        int width, height, nrChannels;
};