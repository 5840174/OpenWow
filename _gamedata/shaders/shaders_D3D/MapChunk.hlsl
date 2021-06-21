#include "CommonInclude.hlsl"

struct VertexShaderInput
{
	float3 position       : POSITION;
	float3 normal         : NORMAL0;
	float3 mccvColor      : COLOR0;
	float4 texCoordDetailAndAlpha : TEXCOORD0;
};

struct VertexShaderOutput
{
	float4 positionVS     : SV_POSITION;
	float4 positionWS     : POSITION;
	float3 normal         : NORMAL0;
	float3 mccvColor      : COLOR0;
	float4 texCoordDetailAndAlpha : TEXCOORD0;
};


// Uniforms
cbuffer Material : register(b2)
{
	uint   LayersCnt;
	bool   IsShadowMapExists;
	bool   IsMCCVExists;
	bool   IsNortrend;
    //-------------------------- ( 16 bytes )
};

// Textures and samples
Texture2D ColorMap0       : register(t0);
Texture2D ColorMap1       : register(t1);
Texture2D ColorMap2       : register(t2);
Texture2D ColorMap3       : register(t3);
Texture2D AlphaMap        : register(t4);
Texture2D SpecMap0        : register(t5);
Texture2D SpecMap1        : register(t6);
Texture2D SpecMap2        : register(t7);
Texture2D SpecMap3        : register(t8);
sampler   ColorMapSampler : register(s0);
sampler   AlphaMapSampler : register(s1);

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(PF.Projection, PF.View/*mul(PF.View, PO.Model)*/);

	VertexShaderOutput OUT;
	OUT.positionVS = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionWS = float4(IN.position, 1.0f);
	OUT.normal = mul(mvp, float4(IN.normal, 0.0f));
	OUT.mccvColor = IN.mccvColor;
	OUT.texCoordDetailAndAlpha = IN.texCoordDetailAndAlpha;
	
	return OUT;
}

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float3 layersColor = float3(0,0,0);
	float4 layersSpec = float4(0,0,0,0);
	float3 resultColor = float3(0,0,0);
	float4 resultSpec = float4(0,0,0,0);
	
	const float4 sampledAlpha = AlphaMap.Sample(AlphaMapSampler, IN.texCoordDetailAndAlpha.zw);
	
if (IsNortrend == 1)
{
	/* NORTREND */
	float alphaSumma = 0.0;
	if (LayersCnt >= 2)
	{
		alphaSumma += sampledAlpha.r;
		layersColor += ColorMap1.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb * sampledAlpha.r;
		layersSpec += SpecMap1.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy) * sampledAlpha.r;
	}
	if (LayersCnt >= 3)
	{
		alphaSumma += sampledAlpha.g;
		layersColor += ColorMap2.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb * sampledAlpha.g;
		layersSpec += SpecMap2.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy) * sampledAlpha.g;
	}
	if (LayersCnt >= 4)
	{
		alphaSumma += sampledAlpha.b;
		layersColor += ColorMap3.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb * sampledAlpha.b;
		layersSpec += SpecMap3.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy) * sampledAlpha.b;
	}
	
	resultColor = ColorMap0.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb * (1.0 - alphaSumma) + layersColor;
	resultSpec = SpecMap0.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy) * (1.0 - alphaSumma) + layersSpec;
}
else
{
	/* NOT NORTREND */
	layersColor = ColorMap0.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb;
	layersSpec = SpecMap0.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy);
	
	if (LayersCnt >= 2)
	{
		layersColor = lerp(layersColor, ColorMap1.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb, sampledAlpha.r);
		layersSpec = lerp(layersSpec, SpecMap1.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy), sampledAlpha.r);
	}
	if (LayersCnt >= 3)
	{
		layersColor = lerp(layersColor, ColorMap2.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb, sampledAlpha.g);
		layersSpec = lerp(layersSpec, SpecMap2.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy), sampledAlpha.g);
	}
	if (LayersCnt >= 4)
	{
		layersColor = lerp(layersColor, ColorMap3.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb, sampledAlpha.b);
		layersSpec = lerp(layersSpec, SpecMap3.Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy), sampledAlpha.b);
	}
	
	resultColor = layersColor;
	resultSpec = layersSpec;	
}

	if (IsShadowMapExists)
	{
		resultColor = lerp(resultColor,  float3(0.0, 0.0, 0.0), sampledAlpha.a);
	}
	
	if (IsMCCVExists)
	{
		resultColor *= float4(IN.mccvColor, 1.0f);
	}
	
	return float4(resultColor, 1.0f);
	
	//DefferedRenderPSOut OUT;
	//OUT.Diffuse = float4(resultColor, 1.0f);
	//OUT.Specular = resultSpec;
	//OUT.NormalVS = float4(IN.normal, 0.0f);
	//return OUT;
}