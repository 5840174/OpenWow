#include "IDB_SHADER_COMMON_TYPES"

struct WMO_Material
{
	uint   gBlendMode;
    //-------------------------- ( 4 bytes )
};

struct VertexShaderInput
{
	float3 position  : POSITION;
	float3 normal    : NORMAL0;
	float4 color     : COLOR0;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};

struct VertexShaderOutput
{
	float4 positionVS: SV_POSITION;
	float4 positionWS: POSITION;
	float3 normalVS  : NORMAL0;
	float4 color     : COLOR0;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};


// Uniforms
cbuffer PerObject : register(b0)
{
	float4x4 Model;
	float4x4 View;
	float4x4 Projection;
}
cbuffer Material : register(b2)
{
    WMO_Material Material;
};

// Textures and samples
Texture2D DiffuseTexture        : register(t0);
sampler   DiffuseTextureSampler : register(s0);

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(Projection, mul(View, Model));
	
	VertexShaderOutput OUT;
	OUT.positionVS = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionWS = float4(IN.position, 1.0f);
	OUT.normalVS = mul(mvp, float4(IN.normal, 0.0f));
	OUT.color = IN.color;
	OUT.texCoord0 = float2(IN.texCoord0.x, 1.0f - IN.texCoord0.y);
	OUT.texCoord1 = IN.texCoord1;
	
	return OUT;
}

PixelShaderOutput PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float4 resultColor = DiffuseTexture.Sample(DiffuseTextureSampler, IN.texCoord0);
	
	if (Material.gBlendMode == 0) // GxBlend_Opaque
	{
		resultColor.a = 1.0f;
	}
	else if (Material.gBlendMode == 1) // GxBlend_AlphaKey
	{
		if (resultColor.a < (224.0f / 255.0f)) discard;
	}
	else 
	{
		if (resultColor.a < (1.0f / 255.0f)) discard;
	}
	
	PixelShaderOutput OUT;
	OUT.PositionWS = float4(IN.positionWS.xyz, /*material*/ 1.0f);
	OUT.Diffuse = resultColor;
	OUT.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	OUT.NormalWS = float4(IN.normalVS, 1.0f);
	return OUT;
}