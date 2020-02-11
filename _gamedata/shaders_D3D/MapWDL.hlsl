#include "IDB_SHADER_COMMON_INCLUDE"

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
cbuffer Material : register(b2)
{
    float4 DiffuseColor;
};

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(PF.Projection, mul(PF.View, PO.Model));

	VertexShaderOutput OUT;
	OUT.positionVS = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionWS = float4(IN.position, 1.0f);
	return OUT;
}

DefferedRenderPSOut PS_main(VertexShaderOutput IN) : SV_TARGET
{
	DefferedRenderPSOut OUT;
	OUT.PositionWS = float4(IN.positionWS.xyz, /*material*/ 0.0f);
	OUT.Diffuse = DiffuseColor;
	OUT.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	OUT.NormalWS = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return OUT;
}