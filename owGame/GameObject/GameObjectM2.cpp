#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "GameObjectM2.h"

GameObjectM2::GameObjectM2(IScene& Scene, CWorldClient& WorldClient, const std::shared_ptr<CM2>& M2Object)
	: CM2_Base_Instance(Scene, WorldClient, M2Object)
{}

GameObjectM2::~GameObjectM2()
{}

void GameObjectM2::Initialize()
{
	__super::Initialize();

	SetDebugDrawColor(ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f));
	SetDebugDrawMode(false);
}

#endif