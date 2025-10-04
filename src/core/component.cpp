#include "component.hpp"

void MeshComponent::Render()
{
    if (auto m = mesh.lock())
        m->Draw();
}