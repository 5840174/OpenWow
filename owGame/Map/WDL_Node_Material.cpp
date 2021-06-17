#include "stdafx.h"

// General
#include "WDL_Node_Material.h"

WDL_Node_Material::WDL_Node_Material(IRenderDevice& RenderDevice) 
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("WDL_Node_Material"))
{
	SetDiffuseColor(glm::vec4(0, 0.2, 0.8, 1.0));
}

WDL_Node_Material::~WDL_Node_Material()
{
}

const glm::vec4& WDL_Node_Material::GetDiffuseColor() const
{
	return MaterialDataReadOnly().m_DiffuseColor;
}

void WDL_Node_Material::SetDiffuseColor(const glm::vec4& diffuse)
{
	MaterialData().m_DiffuseColor = diffuse;
}
