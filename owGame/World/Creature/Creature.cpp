#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "Creature.h"

/*
m_FileName="Creature\\Alexstrasza\\Alexstrasza.m2"

*/

CCreature::CCreature(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: CM2_Base_Instance(Scene, M2Object)
{}

CCreature::~CCreature()
{}



//
// ISceneNode
//
void CCreature::Initialize()
{
	__super::Initialize();

	GetColliderComponent()->SetDebugDrawColor(ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	GetColliderComponent()->SetDebugDrawMode(true);
}

#endif