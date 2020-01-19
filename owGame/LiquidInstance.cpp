#include "stdafx.h"

// General
#include "LiquidInstance.h"

Liquid_Instance::Liquid_Instance()
{}

Liquid_Instance::~Liquid_Instance()
{}



//
// ILiquidInstanceInitializaton
//
void Liquid_Instance::Initialize(std::shared_ptr<CLiquid> LiquidObject, vec3 Position)
{
    // Transform
    {
       SetTranslate(Position);
    }

    // Collider
    std::shared_ptr<IColliderComponent3D> collider = GetComponent<IColliderComponent3D>();
    {
        BoundingBox bbox(vec3(Math::MinFloat, Math::MinFloat, Math::MinFloat), vec3(Math::MaxFloat, Math::MaxFloat, Math::MaxFloat));
        bbox.transform(GetWorldTransfom());
        collider->SetBounds(bbox);
    }

    // Meshes
    std::shared_ptr<IMeshComponent3D> meshes = GetComponent<IMeshComponent3D>();
    for (const auto& it : LiquidObject->m_WaterLayers)
    {
        meshes->AddMesh(it);
    }
}

void Liquid_Instance::Initialize()
{
}

void Liquid_Instance::Finalize()
{
}

