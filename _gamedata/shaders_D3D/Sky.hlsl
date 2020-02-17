#include "IDB_SHADER_COMMON_INCLUDE"

struct VertexShaderInput
{
	float3 position : POSITION;
	float4 color    : COLOR0;
};

struct VertexShaderOutput
{
	float4 positionVS : SV_POSITION;
	float4 positionWS : POSITION;
	float4 color    : COLOR0;
};


// Uniforms
cbuffer Material : register(b2)
{
   uint   gBlendMode;
};


VertexShaderOutput VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(PF.Projection, mul(PF.View, PO.Model));
	
	VertexShaderOutput OUT;
	OUT.positionVS = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionWS = float4(IN.position, 1.0f);
	OUT.color = IN.color;
	return OUT;
}

DefferedRenderPSOut PS_main(VertexShaderOutput IN) : SV_TARGET
{
	DefferedRenderPSOut OUT;
	OUT.Diffuse = float4(IN.color.rgb, 1.0f);
	return OUT;
}