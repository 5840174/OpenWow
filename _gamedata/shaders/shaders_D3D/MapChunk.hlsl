#include "CommonInclude.hlsl"

struct VertexShaderInput
{
	float3 position               : POSITION;
	float3 normal                 : NORMAL0;
	float3 mccvColor              : COLOR0;
	float4 texCoordDetailAndAlpha : TEXCOORD0;
};

struct VertexShaderOutput
{
	float4 positionVS             : SV_POSITION;
	float4 positionWS             : POSITION;
	float3 normal                 : NORMAL0;
	float3 mccvColor              : COLOR0;
	float4 texCoordDetailAndAlpha : TEXCOORD0;
};


// Uniforms
cbuffer Material : register(b2)
{
	uint LayersCnt;
	bool IsShadowMapExists;
	bool IsMCCVExists;
	bool IsNortrend;
    //-------------------------- ( 16 bytes )
};


// Textures
Texture2D ColorMap[4]       : register(t0); // t0 - t3
//Texture2D ColorMap1       : register(t1);
//Texture2D ColorMap2       : register(t2);
//Texture2D ColorMap3       : register(t3);
Texture2D AlphaMap        : register(t4);
Texture2D SpecMap[4]        : register(t5);
//Texture2D SpecMap1        : register(t6);
//Texture2D SpecMap2        : register(t7);
//Texture2D SpecMap3        : register(t8);

// Samplers
sampler   ColorMapSampler : register(s0);
sampler   AlphaMapSampler : register(s1);


VertexShaderOutput VS_main(VertexShaderInput IN)
{
	const float4x4 mvp = mul(PF.Projection, PF.View/*mul(PF.View, PO.Model)*/);

	VertexShaderOutput OUT;
	OUT.positionVS             = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionWS             = float4(IN.position, 1.0f);
	OUT.normal                 = mul(mvp, float4(IN.normal, 0.0f));
	OUT.mccvColor              = IN.mccvColor;
	OUT.texCoordDetailAndAlpha = IN.texCoordDetailAndAlpha;
	
	return OUT;
}


float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float3 resultColor = float3(0.0f, 0.0f, 0.0f);
	float4 resultSpec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	const float4 sampledAlpha = AlphaMap.Sample(AlphaMapSampler, IN.texCoordDetailAndAlpha.zw);
	
	if (IsNortrend) // NORTREND
	{
		float alphaSumma = 0.0f;
		float3 layersColor = float3(0.0f, 0.0f, 0.0f);
		float4 layersSpec = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
		for (uint i = 1u; i < LayersCnt; i++)
		{
			alphaSumma  += sampledAlpha[i - 1];
			layersColor += ColorMap[i].Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb * sampledAlpha[i - 1];
			layersSpec  += SpecMap[i] .Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy)     * sampledAlpha[i - 1];
		}
				
		resultColor = ColorMap[0].Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb * (1.0 - alphaSumma) + layersColor;
		resultSpec  = SpecMap[0] .Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy)     * (1.0 - alphaSumma) + layersSpec;
	}
	else // NOT NORTREND
	{
		float3 layersColor = ColorMap[0].Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb;
		float4 layersSpec  = SpecMap[0] .Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy);
		
		for (uint i = 1u; i < LayersCnt; i++)
		{
			layersColor = lerp(layersColor, ColorMap[i].Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy).rgb, sampledAlpha[i - 1]);
			layersSpec  = lerp(layersSpec,  SpecMap[i] .Sample(ColorMapSampler, IN.texCoordDetailAndAlpha.xy),     sampledAlpha[i - 1]);
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