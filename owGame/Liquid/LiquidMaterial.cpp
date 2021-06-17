#include "stdafx.h"

// General
#include "LiquidMaterial.h"

LiquidMaterial::LiquidMaterial(IRenderDevice& RenderDevice)
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("LiquidMaterial"))
{
}

LiquidMaterial::~LiquidMaterial()
{

}


//
// LiquidMaterial
//
void LiquidMaterial::SetShallowAlpha(float value)
{
	MaterialData().gShallowAlpha = value;
}

void LiquidMaterial::SetDeepAlpha(float value)
{
	MaterialData().gDeepAlpha = value;
}

void LiquidMaterial::SetColorLight(glm::vec3 value)
{
	MaterialData().gColorLight = value;
}

void LiquidMaterial::SetColorDark(glm::vec3 value)
{
	MaterialData().gColorDark = value;
}
