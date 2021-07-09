#include "CommonInclude.hlsl"

struct VertexShaderInput
{
	float3 position  : POSITION;
	float3 normal    : NORMAL0;
	float4 color     : COLOR0;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};

struct VSOutputWMO
{
	float4 position  : SV_POSITION;  // Clip space position.
	float3 normalVS  : NORMAL0;
	float4 color     : COLOR0;
	float4 texCoords : TEXCOORD0;
};


// Uniforms
cbuffer Material : register(b2)
{
    uint   gBlendMode;
	uint   gMOCVExists;
};


// Textures and samples
Texture2D DiffuseTexture        : register(t0);
sampler   DiffuseTextureSampler : register(s0);

VSOutputWMO VS_main(VertexShaderInput IN)
{
	const float4x4 mv = mul(PF.View, PO.Model);
	const float4x4 mvp = mul(PF.Projection, mv);
	
	VSOutputWMO OUT;
	OUT.position = mul(mvp, float4(IN.position, 1.0f));
	OUT.normalVS = mul((float3x3)mv, IN.normal);
	OUT.color = IN.color;
	OUT.texCoords = float4(float2(IN.texCoord0.x, 1.0f - IN.texCoord0.y), float2(IN.texCoord1.x, 1.0f - IN.texCoord1.y));	
	return OUT;
}

DefferedRenderPSOut PS_main(VSOutputWMO IN) : SV_TARGET
{
	float4 resultColor = DiffuseTexture.Sample(DiffuseTextureSampler, IN.texCoords.xy);
	
	if (gBlendMode == 0) // GxBlend_Opaque
	{
		resultColor.a = 1.0f;
	}
	else if (gBlendMode == 1) // GxBlend_AlphaKey
	{
		if (resultColor.a < (224.0f / 255.0f)) discard;
	}
	else 
	{
		if (resultColor.a < (1.0f / 255.0f)) discard;
	}
	
	
	if (gMOCVExists)
	{
		//resultColor = IN.color;
		resultColor = resultColor * 0.5f + (resultColor * IN.color);
		//resultColor *= IN.color;
	}
	
	DefferedRenderPSOut OUT;
	OUT.Diffuse = resultColor;
	OUT.Specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
	OUT.NormalVS = float4(IN.normalVS.xyz, 0.0f);
	return OUT;
}