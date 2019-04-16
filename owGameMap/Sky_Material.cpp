#include "stdafx.h"

// General
#include "Sky_Material.h"

Sky_Material::Sky_Material() :
	MaterialWrapper(_RenderDevice->CreateMaterial())
{
	std::shared_ptr<Shader> g_pVertexShader = _RenderDevice->CreateShader(
		Shader::VertexShader, "shaders_D3D/Sky.hlsl", Shader::ShaderMacros(), "VS_main", "latest"
	);
    g_pVertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<Shader> g_pPixelShader = _RenderDevice->CreateShader(
		Shader::PixelShader, "shaders_D3D/Sky.hlsl", Shader::ShaderMacros(), "PS_main", "latest"
	);

	// Material
	SetShader(Shader::VertexShader, g_pVertexShader);
	SetShader(Shader::PixelShader, g_pPixelShader);
}

Sky_Material::~Sky_Material()
{
}
