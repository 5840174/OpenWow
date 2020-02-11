#include "stdafx.h"

// General
#include "WDL_Node_Material.h"

WDL_Node_Material::WDL_Node_Material(IRenderDevice& RenderDevice) 
	: MaterialProxie(RenderDevice.GetObjectsFactory().CreateMaterial(sizeof(MaterialProperties)))
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();


	// Material
	SetDiffuseColor(vec4(0, 0.2, 0.8, 1.0));

	SetWrapper(this);
}

WDL_Node_Material::~WDL_Node_Material()
{
	_aligned_free(m_pProperties);
}

cvec4 WDL_Node_Material::GetDiffuseColor() const
{
	return m_pProperties->m_DiffuseColor;
}

//-----

void WDL_Node_Material::SetDiffuseColor(cvec4 diffuse)
{
	m_pProperties->m_DiffuseColor = diffuse;
	MarkConstantBufferDirty();
}

void WDL_Node_Material::UpdateConstantBuffer() const
{
	MaterialProxie::UpdateConstantBuffer(m_pProperties, sizeof(MaterialProperties));
}