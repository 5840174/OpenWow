#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "GameObject.h"


GameObject::GameObject(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: CM2_Base_Instance(Scene, M2Object)
{}

GameObject::~GameObject()
{}

#endif