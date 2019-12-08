#include "stdafx.h"

// General
#include "WDL_Node_Material.h"

WDL_Node_Material::WDL_Node_Material(std::shared_ptr<IRenderDevice> RenderDevice) :
	MaterialProxie(RenderDevice->CreateMaterial(sizeof(MaterialProperties)))
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();

	// CreateShaders
	std::shared_ptr<IShader> g_pVertexShader = RenderDevice->CreateShader(
		IShader::ShaderType::VertexShader, "shaders_D3D/MapWDL.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
	);
    g_pVertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> g_pPixelShader = RenderDevice->CreateShader(
		IShader::ShaderType::PixelShader, "shaders_D3D/MapWDL.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
	);

	// Material
	SetDiffuseColor(vec4(0, 0.2, 0.8, 1.0));

	SetShader(IShader::ShaderType::VertexShader, g_pVertexShader);
	SetShader(IShader::ShaderType::PixelShader, g_pPixelShader);
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
	MaterialProxie::UpdateConstantBuffer(m_pProperties, sizeof(MaterialProperties));
}