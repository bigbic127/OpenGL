#include "material.hpp"


void Material::Apply()
{
    if(auto s = shader.lock())
    {
        s->UseProgram();
        s->SetMatrix4("mModel", mModel);
        s->SetMatrix4("mView", mView);
        s->SetMatrix4("mProjection", mProjection);
        s->SetVector3("baseColor", baseColor);
        //Texture
        for(auto& texture:textures)
        {
            if(auto t = texture.lock())
            {
                t->Bind();
            }
        }
    }
}

void Material::AddTexture(std::shared_ptr<Texture> texture, std::string name)
{
    if(auto s = shader.lock())
    {
        texture->Init(s, name);
        textures.push_back(texture);
    }
}
