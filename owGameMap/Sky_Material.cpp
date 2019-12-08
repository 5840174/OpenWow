#include "stdafx.h"

// General
#include "Sky_Material.h"

Sky_Material::Sky_Material(std::shared_ptr<IRenderDevice> RenderDevice) 
	: MaterialProxie(RenderDevice->CreateMaterial(0))
{
    std::shared_ptr<IShader> g_pVertexShader;
    std::shared_ptr<IShader> g_pPixelShader;

    if (RenderDevice->GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
    {
        g_pVertexShader = RenderDevice->CreateShader(
            IShader::ShaderType::VertexShader, "shaders_D3D/Sky.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
        );
        g_pPixelShader = RenderDevice->CreateShader(
            IShader::ShaderType::PixelShader, "shaders_D3D/Sky.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
        );
    }
    else if (RenderDevice->GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
    {
        g_pVertexShader = RenderDevice->CreateShader(
            IShader::ShaderType::VertexShader, "shaders_OGL/Sky.vs", IShader::ShaderMacros(), "", ""
        );
        g_pPixelShader = RenderDevice->CreateShader(
            IShader::ShaderType::PixelShader, "shaders_OGL/Sky.ps", IShader::ShaderMacros(), "", ""
        );
    }
    g_pVertexShader->LoadInputLayoutFromReflector();

	// Material
	SetShader(IShader::ShaderType::VertexShader, g_pVertexShader);
	SetShader(IShader::ShaderType::PixelShader, g_pPixelShader);
}

Sky_Material::~Sky_Material()
{
}
