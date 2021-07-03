#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "GameObjectM2.h"

GameObjectM2::GameObjectM2(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: CM2_Base_Instance(Scene, M2Object)
{}

GameObjectM2::~GameObjectM2()
{}

void GameObjectM2::Initialize()
{
	__super::Initialize();

	GetColliderComponent()->SetDebugDrawColor(ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f));
	GetColliderComponent()->SetDebugDrawMode(true);
}

#endif