#include "IDB_SHADER_COMMON_TYPES"

struct MapWDL_Material
{
    float4 DiffuseColor;
};

struct VertexShaderInput
{
	float3 position       : POSITION;
};
struct VertexShaderOutput
{
	float4 positionVS : SV_POSITION;
	float4 positionWS : POSITION;
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
    MapWDL_Material Material;
};

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(Projection, mul(View, Model));

	VertexShaderOutput OUT;
	OUT.positionVS = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionWS = float4(IN.position, 1.0f);
	return OUT;
}

PixelShaderOutput PS_main(VertexShaderOutput IN) : SV_TARGET
{
	PixelShaderOutput OUT;
	OUT.PositionWS = float4(IN.positionWS.xyz, /*material*/ 0.0f);
	OUT.Diffuse = Material.DiffuseColor;
	OUT.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	OUT.NormalWS = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return OUT;
}