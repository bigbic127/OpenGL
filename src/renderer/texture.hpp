#pragma once
#include <glad/glad.h>
#include <string>
#include <memory>
#include "shader.hpp"

class Texture
{
    public:
        Texture(std::string path);
        Texture(const unsigned char* data, unsigned int size);
        ~Texture();
        void Init(std::shared_ptr<Shader> shader, std::string& name, unsigned int index);
        void Bind();
        void Unbind();
    private:
        unsigned int textureID, locationID;
        int width, height, nrChannels;
};