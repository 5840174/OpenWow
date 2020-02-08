#include "stdafx.h"

// Include
#include "M2_Base_Instance.h"

// General
#include "M2_ColliderComponent.h"

CM2_ColliderComponent::CM2_ColliderComponent(const ISceneNode3D& OwnerNode)
    : CColliderComponent3D(OwnerNode)
{
}

CM2_ColliderComponent::~CM2_ColliderComponent()
{
}

const CM2_Base_Instance& CM2_ColliderComponent::GetOwnerNode()
{
    return reinterpret_cast<const CM2_Base_Instance&>(__super::GetOwnerNode());
}



//
// Protected
//
void CM2_ColliderComponent::UpdateBounds()
{
    std::shared_ptr<M2> m2Model = GetOwnerNode().getM2();
    if (m2Model)
    {
        BoundingBox bbox = m2Model->GetBounds();
        bbox.transform(GetOwnerNode().GetWorldTransfom());
        GetComponent<CColliderComponent3D>()->SetBounds(bbox);
    }
    else
    {
        BoundingBox bbox;
        bbox.calculateCenter();
        GetComponent<CColliderComponent3D>()->SetBounds(bbox);
    }
}
