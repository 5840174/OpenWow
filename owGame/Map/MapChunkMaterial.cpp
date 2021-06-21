#include "stdafx.h"

// Include
#include "MapTile.h"

// General
#include "MapChunkMaterial.h"


CMapChunkMaterial::CMapChunkMaterial(IRenderDevice& RenderDevice) 
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("CMapChunkMaterial"))
{
}

CMapChunkMaterial::~CMapChunkMaterial()
{
}

//--

void CMapChunkMaterial::SetLayersCnt(uint32 value)
{
	MaterialData().LayersCnt = value;
}

void CMapChunkMaterial::SetIsShadowMapExists(uint32 value)
{
	MaterialData().IsShadowMapExists = value;
}

void CMapChunkMaterial::SetIsMCCVExists(uint32 value)
{
	MaterialData().IsMCCVExists = value;
}

void CMapChunkMaterial::SetIsNortrend(uint32 value)
{
	MaterialData().IsNortrend = value;
}
