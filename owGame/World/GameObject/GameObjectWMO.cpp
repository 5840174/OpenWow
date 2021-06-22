#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "GameObjectWMO.h"


GameObjectWMO::GameObjectWMO(IScene& Scene, const std::shared_ptr<CWMO>& M2Object)
	: CWMO_Base_Instance(Scene, M2Object)
{
}

GameObjectWMO::~GameObjectWMO()
{}

void GameObjectWMO::Initialize()
{
	__super::Initialize();

	GetColliderComponent()->SetDebugDrawColor(ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f));
	GetColliderComponent()->SetDebugDrawMode(true);
}

#endif