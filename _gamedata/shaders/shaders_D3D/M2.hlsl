#include "CommonInclude.hlsl"

struct M2PerObject
{
	float4x4 Model;
	float4   Color;
};

struct VertexShaderInput
{
	float3 position  : POSITION;
	float4 boneWeight: BLENDWEIGHT0;
	uint4  boneIndex : BLENDINDICES0;
	float3 normal    : NORMAL0;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};

struct VSOutputM2
{
	float4 position   : SV_POSITION;  // Clip space position.
	float4 color      : COLOR0;    
	float3 normalVS   : NORMAL0;
	float4 texCoords  : TEXCOORD0;
};

#include "shaders_D3D\\M2Utils.hlsl"


// Uniforms
cbuffer Material : register(b2)
{
	uint      gBlendMode;
	uint      gShader;
	float     gTextureWeight;
	uint      gTextureAnimEnable;
	//--------------------------------------------------------------(16 bytes)
	float4    gMaterialColorAndAlpha;
	//--------------------------------------------------------------(16 bytes)
	float4x4  gTextureAnimMatrix;
	//--------------------------------------------------------------(16 * 4 bytes)
};


// Uniforms
cbuffer M2PerObject : register(b6)
{
	M2PerObject M2PO;
}
cbuffer M2Geometry  : register(b7)
{
	bool gIsAnimated;
	uint gStartBoneIndex;
	uint gBonesMaxInfluences;
	float __padding1;
};


// Textures
Texture2D DiffuseTexture0        : register(t0);
Texture2D DiffuseTexture1        : register(t1);


// Samplers
sampler DiffuseTexture0Sampler : register(s0);
sampler DiffuseTexture1Sampler : register(s1);


StructuredBuffer<M2PerObject> Instances : register(t3);
StructuredBuffer<float4x4>    Bones     : register(t4);



VSOutputM2 DoVSRender(VertexShaderInput IN, M2PerObject M2PerObject)
{
	float4 vertexPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 vertexNormal = float3(0.0f, 0.0f, 0.0f);
	
	if (gIsAnimated && gBonesMaxInfluences > 0u)
	{	
		for (uint i = 0u; i < gBonesMaxInfluences; i++)
		{
			if (IN.boneWeight[i] > 0.0f)
			{
				//uint boneIndexes[4];
				//boneIndexes[0] = (IN.boneIndex & 0xFF000000u >> 24) & 0x000000FFu;
				//boneIndexes[1] = (IN.boneIndex & 0x00FF0000u >> 16) & 0x000000FFu;
				//boneIndexes[2] = (IN.boneIndex & 0x0000FF00u >>  8) & 0x000000FFu;
				//boneIndexes[3] = (IN.boneIndex & 0x000000FFu      ) & 0x000000FFu;
				
				const float4x4 boneMatrix = Bones[(IN.boneIndex[i])];
				vertexPosition += mul(boneMatrix, float4(IN.position, 1.0f) * IN.boneWeight[i]);
				vertexNormal += mul((float3x3)boneMatrix, IN.normal * IN.boneWeight[i]);
			}
		}
	}
	else
	{
		vertexPosition = float4(IN.position, 1.0f);
		vertexNormal = IN.normal;
	}

	const float4x4 mv = mul(PF.View, M2PerObject.Model);
	const float4x4 mvp = mul(PF.Projection, mv);

	VSOutputM2 OUT;
	OUT.position  = mul(mvp, vertexPosition);
	OUT.color     = M2PerObject.Color;
	OUT.normalVS  = mul((float3x3)mv, vertexNormal);
	OUT.texCoords = float4(IN.texCoord0, IN.texCoord1);
	return OUT;
}

VSOutputM2 VS_main(VertexShaderInput IN)
{
	return DoVSRender(IN, M2PO);
}

VSOutputM2 VS_main_Inst(VertexShaderInput IN, uint InstanceID : SV_InstanceID)
{
	return DoVSRender(IN, Instances[InstanceID]);
}



/*
 *	Pixel shader
*/

static const float cAlphaThreshold_AlphaKey = 224.0f / 255.0f;
static const float cAlphaThreshold_TransperentKey = 1.0f / 255.0f;

DefferedRenderPSOut PS_main(VSOutputM2 IN) : SV_TARGET
{
	float3 colorRGB = /*IN.color.rgb **/ gMaterialColorAndAlpha.rgb;
	float  colorA   = /*IN.color.a   **/ gMaterialColorAndAlpha.a * gTextureWeight;
 	
	if (colorA < cAlphaThreshold_TransperentKey)
		discard;

	float2 tc0 = (gTextureAnimEnable != 0) ? mul(gTextureAnimMatrix, float4(IN.texCoords.xy, 0.0f, 1.0f)).xy : IN.texCoords.xy;
	float2 tc1 = /*(gTextureAnimEnable != 0) ? mul(gTextureAnimMatrix, float4(IN.texCoords.xy, 0.0f, 1.0f)).xy :*/ IN.texCoords.xy;

	float4 resultColor = Test(gShader, float4(colorRGB, colorA), DiffuseTexture0, DiffuseTexture0Sampler, tc0, DiffuseTexture1, DiffuseTexture1Sampler, tc1);
	
	if (gBlendMode == 0) // GxBlend_Opaque
	{
		resultColor.a = 1.0f;
	}
	else if (gBlendMode == 1) // GxBlend_AlphaKey
	{
		if (resultColor.a < cAlphaThreshold_AlphaKey) 
			discard;
	}
	else 
	{
		if (resultColor.a < cAlphaThreshold_TransperentKey) 
			discard;
	}
	
	DefferedRenderPSOut OUT;
	OUT.Diffuse = resultColor;
	OUT.Specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
	OUT.NormalVS = float4(IN.normalVS.xyz, 0.0f);
	return OUT;
}
