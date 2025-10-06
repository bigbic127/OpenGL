#include "material.hpp"


void Material::Apply()
{
    if(auto s = shader.lock())
    {
        s->UseProgram();
        s->SetVector3("baseColor", baseColor);
    }
}