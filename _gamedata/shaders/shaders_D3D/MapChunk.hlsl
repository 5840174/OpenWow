#include "CommonInclude.hlsl"

struct VertexShaderInput
{
	float3 position               : POSITION;
	float3 normal                 : NORMAL0;
	float3 mccvColor              : COLOR0;
	float4 texCoordDetailAndAlpha : TEXCOORD0;
};

struct VSOutputMapChunk
{
	float4 position               : SV_POSITION;
	float3 normalVS               : NORMAL0;
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
Texture2D ColorMap[4] : register(t0); // t0 - t3
Texture2D AlphaMap    : register(t4);
Texture2D SpecMap[4]  : register(t5);

// Samplers
sampler   ColorMapSampler : register(s0);
sampler   AlphaMapSampler : register(s1);


VSOutputMapChunk VS_main(VertexShaderInput IN)
{
	const float4x4 mv = PF.View;
	const float4x4 mvp = mul(PF.Projection, mv);

	VSOutputMapChunk OUT;
	OUT.position               = mul(mvp, float4(IN.position, 1.0f));
	OUT.normalVS               = mul((float3x3)mv, IN.normal);
	OUT.mccvColor              = IN.mccvColor;
	OUT.texCoordDetailAndAlpha = IN.texCoordDetailAndAlpha;
	return OUT;
}


DefferedRenderPSOut PS_main(VSOutputMapChunk IN) : SV_TARGET
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
	
	DefferedRenderPSOut OUT;
	OUT.Diffuse = float4(resultColor, 1.0f);
	OUT.Specular = resultSpec;
	OUT.NormalVS = float4(IN.normalVS, 0.0f);
	return OUT;
}