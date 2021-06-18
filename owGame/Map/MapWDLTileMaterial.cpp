#include "stdafx.h"

// General
#include "MapWDLTileMaterial.h"

CMapWDLTileMaterial::CMapWDLTileMaterial(IRenderDevice& RenderDevice) 
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("CMapWDLTileMaterial"))
{
	SetDiffuseColor(glm::vec4(0.0f, 0.2f, 0.8f, 0.7f));
}

CMapWDLTileMaterial::~CMapWDLTileMaterial()
{
}

const glm::vec4& CMapWDLTileMaterial::GetDiffuseColor() const
{
	return MaterialDataReadOnly().m_DiffuseColor;
}

void CMapWDLTileMaterial::SetDiffuseColor(const glm::vec4& diffuse)
{
	MaterialData().m_DiffuseColor = diffuse;
}
