#include "stdafx.h"

// General
#include "LiquidBaseInstance.h"

CLiquidBaseInstance::CLiquidBaseInstance(IScene& Scene)
	: CSceneNode(Scene)
{
	SetUpdateEnabled(false);
}

CLiquidBaseInstance::~CLiquidBaseInstance()
{}



//
// ISceneNode
//
void CLiquidBaseInstance::Initialize()
{
	__super::Initialize();

	SetCullStrategy(ECullStrategy::ByFrustrum);
	SetDebugDrawMode(false);
	SetDebugDrawColor(ColorRGBA(0.3f, 0.3f, 0.8f, 0.8f));
}
