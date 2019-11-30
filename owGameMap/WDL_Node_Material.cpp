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
	std::shared_ptr<IShader> g_pVertexShader = _RenderDevice->CreateShader(
		IShader::ShaderType::VertexShader, "shaders_D3D/MapWDL.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
	);
    g_pVertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> g_pPixelShader = _RenderDevice->CreateShader(
		IShader::PixelShader, "shaders_D3D/MapWDL.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
	);

	// Material
	SetDiffuseColor(vec4(0, 0.2, 0.8, 1.0));

	SetShader(IShader::ShaderType::VertexShader, g_pVertexShader);
	SetShader(IShader::PixelShader, g_pPixelShader);
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