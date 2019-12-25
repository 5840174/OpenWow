#include "IDB_SHADER_COMMON_TYPES"

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
cbuffer PerObject : register(b0)
{
	float4x4 Model;
	float4x4 View;
	float4x4 Projection;
}
cbuffer Material : register(b2)
{
   uint   gBlendMode;
};


VertexShaderOutput VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(Projection, mul(View, Model));
	
	VertexShaderOutput OUT;
	OUT.positionVS = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionWS = float4(IN.position, 1.0f);
	OUT.color = IN.color;
	return OUT;
}

PixelShaderOutput PS_main(VertexShaderOutput IN) : SV_TARGET
{
	PixelShaderOutput OUT;
	OUT.PositionWS = float4(IN.positionWS.xyz, /*material*/ 0.0f);
	OUT.Diffuse = float4(IN.color.rgb, 1.0f);
	return OUT;
}