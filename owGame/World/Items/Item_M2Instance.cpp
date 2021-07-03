#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "Item_M2Instance.h"

CItem_M2Instance::CItem_M2Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: CM2_Base_Instance(Scene, M2Object)
{}

CItem_M2Instance::~CItem_M2Instance()
{}

void CItem_M2Instance::AddVisualEffect(std::shared_ptr<CM2_Base_Instance> _visualEffect)
{
	AddChild(_visualEffect);
}

#endif