#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "M2_Base_Instance.h"

// General
#include "M2_ColliderComponent.h"

CM2_ColliderComponent::CM2_ColliderComponent(const ISceneNode& OwnerNode)
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
    //GetComponentT<IColliderComponent>()->SetBounds(bbox);
	__super::UpdateBounds();
}

#endif