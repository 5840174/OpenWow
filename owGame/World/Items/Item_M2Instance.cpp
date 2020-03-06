#include "stdafx.h"

// General
#include "Item_M2Instance.h"

CItem_M2Instance::CItem_M2Instance(const std::shared_ptr<CM2>& M2Object)
	: CM2_Base_Instance(M2Object)
{
}

CItem_M2Instance::~CItem_M2Instance()
{
}

void CItem_M2Instance::AddVisualEffect(std::shared_ptr<CM2_Base_Instance> _visualEffect)
{
	AddChild(_visualEffect);
}
