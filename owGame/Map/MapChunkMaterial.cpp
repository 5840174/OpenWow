#include "stdafx.h"

// Include
#include "MapTile.h"

// General
#include "MapChunkMaterial.h"


ADT_MCNK_Material::ADT_MCNK_Material(IRenderDevice& RenderDevice) 
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("ADT_MCNK_Material"))
{
}

ADT_MCNK_Material::~ADT_MCNK_Material()
{
}

//--

void ADT_MCNK_Material::SetLayersCnt(uint32 value)
{
	MaterialData().LayersCnt = value;
}

void ADT_MCNK_Material::SetIsShadowMapExists(uint32 value)
{
	MaterialData().IsShadowMapExists = value;
}

void ADT_MCNK_Material::SetIsMCCVExists(uint32 value)
{
	MaterialData().IsMCCVExists = value;
}

void ADT_MCNK_Material::SetIsNortrend(uint32 value)
{
	MaterialData().IsNortrend = value;
}
