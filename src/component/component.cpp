#include "component.hpp"

void MeshComponent::Render()
{
    if(auto m = mesh.lock())
    {
        m->Bind();
        if(auto mat = material.lock())
        {
            mat->Apply();
        }
        m->Draw();
        m->Unbind();
    }
}