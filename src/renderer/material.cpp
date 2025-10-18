#include "material.hpp"
#include "core/logger.hpp"

void Material::Apply()
{
    if(auto s = shader.lock())
    {
        s->SetVector3("ambientColor", parameter.ambientColor);
        s->SetVector3("baseColor", parameter.baseColor);
        s->SetVector3("specularColor", parameter.specularColor);
        s->SetFloat("specularShininess", parameter.specularShininess);
        s->SetBool("bDiffuse", int(parameter.bDiffuse));
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
        //Camera
        s->SetVector3("cameraPosition", vCameraPosition);
        //Parameter
        Apply();
    }
}

void Material::Unbind()
{
    if(auto s = shader.lock())
    {
        s->Unbind();
        if(parameter.bDiffuse)
        {
            if(auto p = parameter.diffuseTexture.lock())
                p->Unbind();
        }
    }
}

void Material::AddTexture(std::shared_ptr<Texture> texture, std::string name)
{
    if(auto s = shader.lock())
    {
        if (name == "diffuseTexture")
        {
            texture->Init(s, name, 0);
            parameter.diffuseTexture = texture;
            parameter.bDiffuse = true;
        }
        else if(name == "specularTexture")
        {
            texture->Init(s, name, 1);
            parameter.specularTexture = texture;
            parameter.bSpecular = true;
        }
    }
}
