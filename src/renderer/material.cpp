#include "material.hpp"
#include "core/logger.hpp"

void Material::Apply()
{
    if(auto s = shader.lock())
    {
        s->SetVector3("ambientColor", parameter.ambientColor);
        s->SetVector3("baseColor", parameter.baseColor);
        s->SetBool("bDiffuse", int(parameter.bDiffuse));
        s->SetVector3("specColor", parameter.specColor);
        s->SetBool("bSpecular", int(parameter.bSpecular));
        s->SetBool("bNormal", int(parameter.bSpecular));
        if(parameter.bDiffuse)
        {
            if(auto p = parameter.diffuseTexture.lock())
                p->Bind();
        }
    }
}

void Material::Bind()
{
    if(auto s = shader.lock())
    {
        s->UseProgram();
        s->SetMatrix4("mModel", mModel);
        s->SetMatrix4("mView", mView);
        s->SetMatrix4("mProjection", mProjection);
        //Light
        s->SetVector3("lightPosition", vLightPosition);
        s->SetVector3("directionalLight", vLightDirectional);
        s->SetFloat("lightIntensity", fLightIntensity);
        s->SetVector3("lightColor", vLightColor);
        //Parameter
        Apply();
    }
}

void Material::Unbind()
{
    if(parameter.bDiffuse)
    {
        if(auto p = parameter.diffuseTexture.lock())
            p->Bind();
    }
}

void Material::AddTexture(std::shared_ptr<Texture> texture, std::string name)
{
    if(auto s = shader.lock())
    {
        texture->Init(s, name);
        if (name == "diffuseTexture")
            parameter.diffuseTexture = texture;
    }
}
