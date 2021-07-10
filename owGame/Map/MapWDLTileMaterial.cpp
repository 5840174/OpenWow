#include "stdafx.h"

// General
#include "MapWDLTileMaterial.h"

CMapWDLTileMaterial::CMapWDLTileMaterial(IRenderDevice& RenderDevice) 
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("CMapWDLTileMaterial"))
{
	SetDiffuseColor(ColorRGBA(0.0f, 0.2f, 0.8f, 0.7f));
}

CMapWDLTileMaterial::~CMapWDLTileMaterial()
{
}

const ColorRGBA& CMapWDLTileMaterial::GetDiffuseColor() const
{
	return MaterialDataReadOnly().DiffuseColor;
}

void CMapWDLTileMaterial::SetDiffuseColor(const ColorRGBA& diffuse)
{
	MaterialData().DiffuseColor = diffuse;
}
