#include "IDB_SHADER_COMMON_TYPES"

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
cbuffer PerObject : register(b0)
{
	float4x4 Model;
	float4x4 View;
	float4x4 Projection;
}
cbuffer Material : register(b2)
{
	float gShallowAlpha;
	float gDeepAlpha;
	float2 pad0;
	
	float3 gColorLight;
	float pad1;
	
	float3 gColorDark;
	float pad2;
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
	OUT.texCoord = IN.texCoord;
	return OUT;
}

PixelShaderOutput PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float alpha = DiffuseTexture.Sample(DiffuseTextureSampler, IN.texCoord).w;

	float4 resultColor = float4(gColorLight, (1.0 - IN.texCoord.z) * gShallowAlpha) + float4(gColorDark, IN.texCoord.z * gDeepAlpha);
	resultColor *= (1.0 - alpha);
	resultColor += float4(1.0f, 1.0f, 1.0f, 1.0f) * alpha;

	PixelShaderOutput OUT;
	OUT.PositionWS = IN.positionWS;
	OUT.Diffuse = resultColor;
	OUT.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	OUT.NormalWS = float4(0.0f, 1.0f, 0.0f, 1.0f);
	return OUT;
}