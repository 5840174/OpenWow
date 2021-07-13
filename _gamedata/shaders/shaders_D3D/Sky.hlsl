#include "CommonInclude.hlsl"

struct VSInputSky
{
	float3 position : POSITION;
	float4 color    : COLOR0;
};

struct VSOutputSky
{
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
};

VSOutputSky VS_main(VSInputSky IN)
{
	const float4x4 mvp = mul(PF.Projection, mul(PF.View, PO.Model));
	
	VSOutputSky OUT;
	OUT.position = mul(mvp, float4(IN.position, 1.0f));
	OUT.color = IN.color;
	return OUT;
}

DefferedRenderPSOut PS_main(VSOutputSky IN) : SV_TARGET
{
	DefferedRenderPSOut OUT;
	OUT.Diffuse = IN.color;
	OUT.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	OUT.NormalVS = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return OUT;
}