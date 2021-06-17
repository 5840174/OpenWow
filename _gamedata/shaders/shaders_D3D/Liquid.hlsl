#include "CommonInclude.hlsl"

struct VertexShaderInput
{
	float3 position : POSITION;
	float3 texCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
	float4 positionVS : SV_POSITION;
	float4 positionWS : POSITION;
	float3 texCoord : TEXCOORD0;
};


// Uniforms
cbuffer Material : register(b2)
{
	float3 gColorLight;
	float gShallowAlpha;
	
	float3 gColorDark;
	float gDeepAlpha;
};

// Textures and samples
Texture2D DiffuseTexture        : register(t0);
sampler   DiffuseTextureSampler : register(s0);

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(PF.Projection, mul(PF.View, PO.Model));
	
	VertexShaderOutput OUT;
	OUT.positionVS = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionWS = float4(IN.position, 1.0f);
	OUT.texCoord = IN.texCoord;
	return OUT;
}

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float alpha = DiffuseTexture.Sample(DiffuseTextureSampler, IN.texCoord.xy).w;

	float4 resultColor = float4(gColorLight, (1.0 - IN.texCoord.z) * gShallowAlpha) + float4(gColorDark, IN.texCoord.z * gDeepAlpha);
	resultColor *= (1.0 - alpha);
	resultColor += float4(1.0f, 1.0f, 1.0f, 1.0f) * alpha;

	return resultColor;
	
	//DefferedRenderPSOut OUT;
	//OUT.Diffuse = resultColor;
	//OUT.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	//OUT.NormalVS = float4(0.0f, 1.0f, 0.0f, 1.0f);
	//return OUT;
}