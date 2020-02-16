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
    //BoundingBox bbox = GetOwnerNode().getM2().GetBounds();
    //bbox.transform(GetOwnerNode().GetWorldTransfom());
    //GetComponent<CColliderComponent3D>()->SetBounds(bbox);
}
