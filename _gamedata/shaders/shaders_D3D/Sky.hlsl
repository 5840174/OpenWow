#include "CommonInclude.hlsl"

struct VertexShaderInput
{
	float3 position : POSITION;
	float4 color    : COLOR0;
};

struct VSOutputSky
{
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
};


// Uniforms
cbuffer Material : register(b2)
{
   uint   gBlendMode;
};


VSOutputSky VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(PF.Projection, mul(PF.View, PO.Model));
	
	VSOutputSky OUT;
	OUT.position = mul(mvp, float4(IN.position, 1.0f));
	OUT.color = IN.color;
	return OUT;
}

float4 PS_main(VSOutputSky IN) : SV_TARGET
{
	return IN.color;
}