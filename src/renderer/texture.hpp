#pragma once
#include <glad/glad.h>
#include <string>
#include <memory>
#include "shader.hpp"

class Texture
{
    public:
        Texture(std::string path);//이미지 저장 경로
        Texture(const unsigned char* data, unsigned int size);//byte 압축(png,jpg)
        Texture(const unsigned char* data, unsigned int width, unsigned int height);//byte 비압축(row)
        ~Texture();
        void Init(std::shared_ptr<Shader> shader, std::string& name, unsigned int index);
        void Bind();
        void Unbind();
    private:
        unsigned int textureID, locationID;
        int width, height, nrChannels;
};