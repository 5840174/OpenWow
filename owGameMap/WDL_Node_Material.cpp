#include "stdafx.h"

// General
#include "WDL_Node_Material.h"

WDL_Node_Material::WDL_Node_Material() :
	MaterialWrapper(_RenderDevice->CreateMaterial(sizeof(MaterialProperties)))
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();

#ifdef IS_DX11
	// CreateShaders
	std::shared_ptr<Shader> g_pVertexShader = _RenderDevice->CreateShader(
		Shader::VertexShader, "shaders_D3D/MapWDL.hlsl", Shader::ShaderMacros(), "VS_main", "latest"
	);
    g_pVertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<Shader> g_pPixelShader = _RenderDevice->CreateShader(
		Shader::PixelShader, "shaders_D3D/MapWDL.hlsl", Shader::ShaderMacros(), "PS_main", "latest"
	);

	// Material
	SetDiffuseColor(vec4(0, 0.2, 0.8, 1.0));

	SetShader(Shader::VertexShader, g_pVertexShader);
	SetShader(Shader::PixelShader, g_pPixelShader);
#endif
}

WDL_Node_Material::~WDL_Node_Material()
{
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
	MaterialWrapper::UpdateConstantBuffer(m_pProperties, sizeof(MaterialProperties));
}