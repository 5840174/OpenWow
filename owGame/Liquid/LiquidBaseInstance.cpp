#include "stdafx.h"

// General
#include "LiquidBaseInstance.h"

CLiquidBaseInstance::CLiquidBaseInstance(IScene& Scene)
	: CSceneNode(Scene)
{

}

CLiquidBaseInstance::~CLiquidBaseInstance()
{}



//
// ISceneNode
//
void CLiquidBaseInstance::Initialize()
{
	__super::Initialize();

	if (auto colliderComponent = GetComponentT<IColliderComponent>())
	{
		colliderComponent->SetCullStrategy(IColliderComponent::ECullStrategy::ByFrustrum);
		colliderComponent->SetDebugDrawMode(false);
		colliderComponent->SetDebugDrawColor(ColorRGBA(0.3f, 0.3f, 0.8f, 0.8f));
	}
}
