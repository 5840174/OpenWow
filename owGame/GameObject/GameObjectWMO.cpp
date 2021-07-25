#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "GameObjectWMO.h"

GameObjectWMO::GameObjectWMO(IScene& Scene, CWorldClient& WorldClient, const std::shared_ptr<CWMO>& M2Object)
	: CWMO_Base_Instance(Scene, WorldClient, M2Object)
{}

GameObjectWMO::~GameObjectWMO()
{}

void GameObjectWMO::Initialize()
{
	__super::Initialize();

	SetDebugDrawColor(ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f));
	SetDebugDrawMode(false);
}

#endif