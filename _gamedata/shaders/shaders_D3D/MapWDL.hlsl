#include "CommonInclude.hlsl"


// Uniforms
cbuffer Material : register(b2)
{
    float4 DiffuseColor;
};

float4 VS_main(VSInputP IN) : SV_POSITION
{
	const float4x4 mvp = mul(PF.Projection, PF.View);
	return mul(mvp, float4(IN.position, 1.0f));
}

float4 PS_main(float4 PositionWVP : SV_POSITION) : SV_TARGET
{
	return DiffuseColor;
	
	//DefferedRenderPSOut OUT;
	//OUT.Diffuse = DiffuseColor;
	//OUT.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	//OUT.NormalVS = float4(1.0f, 1.0f, 1.0f, 1.0f);
	//return OUT;
}