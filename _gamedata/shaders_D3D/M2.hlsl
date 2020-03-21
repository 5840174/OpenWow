#include "IDB_SHADER_COMMON_INCLUDE"

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

struct VertexShaderOutput
{
	float4 positionVS : SV_POSITION;
	float4 positionWS : POSITION;
	float4 color      : COLOR0;    
	float3 normal     : NORMAL0;
	float2 texCoord0  : TEXCOORD0;
	float2 texCoord1  : TEXCOORD1;
};

// Uniforms
cbuffer Material : register(b2)
{
	uint      gBlendMode;
	uint      gShader;
	bool      gTextureAnimEnable;
	float     gTextureWeight;
	//--------------------------------------------------------------( 16 bytes )
		
	float4    gColor;
	//--------------------------------------------------------------( 16 bytes )
	
	float4x4  gTextureAnimMatrix;
	//--------------------------------------------------------------( 16 * 4 bytes )
};

// Uniforms

cbuffer M2PerObject : register(b6)
{
	M2PerObject M2PO;
}

cbuffer M2Geometry : register(b7)
{
	uint gIsAnimated;
	uint gStartBoneIndex;
	uint gBonesMaxInfluences;
	float __padding1;
};

// Textures and samples
Texture2D DiffuseTexture0        : register(t0);
Texture2D DiffuseTexture1        : register(t1);

sampler   DiffuseTexture0Sampler : register(s0);
sampler   DiffuseTexture1Sampler : register(s1);

StructuredBuffer<M2PerObject> Instances  : register(t3);
StructuredBuffer<float4x4> Bones  : register(t4);

float4 MixColorAndTexture(uint BlendMode, float4 _in, float4 tex0);

VertexShaderOutput DoPSRender(VertexShaderInput IN, M2PerObject M2PerObject)
{
	float4 newVertex = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if (gIsAnimated && gBonesMaxInfluences > 0u)
	{	
		for (uint i = 0; i < gBonesMaxInfluences; i++)
		{
			if (IN.boneWeight[i] > 0.0f)
			{
				//uint boneIndexes[4];
				//boneIndexes[0] = (IN.boneIndex & 0xFF000000u >> 24) & 0x000000FFu;
				//boneIndexes[1] = (IN.boneIndex & 0x00FF0000u >> 16) & 0x000000FFu;
				//boneIndexes[2] = (IN.boneIndex & 0x0000FF00u >>  8) & 0x000000FFu;
				//boneIndexes[3] = (IN.boneIndex & 0x000000FFu      ) & 0x000000FFu;
			
				newVertex += mul(Bones[(IN.boneIndex[i] - gStartBoneIndex)], float4(IN.position, 1.0f) * IN.boneWeight[i]);
			}
		}
	}
	else
	{
		newVertex = float4(IN.position, 1.0f);
	}

	const float4x4 mvp = mul(PF.Projection, mul(PF.View, M2PerObject.Model));

	VertexShaderOutput OUT;
	OUT.positionVS = mul(mvp, newVertex);
	OUT.positionWS = newVertex;
	OUT.color = M2PerObject.Color;
	OUT.normal = mul(mvp, IN.normal);
	if (gTextureAnimEnable)
	{
		OUT.texCoord0 = mul(gTextureAnimMatrix, float4(IN.texCoord0, 1.0f, 1.0f)).xy;
		OUT.texCoord1 = IN.texCoord0; //mul(gTextureAnimMatrix, float4(IN.texCoord1, 1.0f, 1.0f)).xy;
	}
	else
	{
		OUT.texCoord0 = IN.texCoord0;
		OUT.texCoord1 = IN.texCoord0;
	}
	return OUT;
}

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	return DoPSRender(IN, M2PO);
}

VertexShaderOutput VS_main_Inst(VertexShaderInput IN, uint InstanceID : SV_InstanceID)
{
	return DoPSRender(IN, Instances[InstanceID]);
}


DefferedRenderPSOut PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float4 resultColor = DiffuseTexture0.Sample(DiffuseTexture0Sampler, float2(IN.texCoord0.x, 1.0f - IN.texCoord0.y));
	
	if (gBlendMode == 0) // GxBlend_Opaque
	{
		resultColor.a = 1.0f;
	}
	else if (gBlendMode == 1) // GxBlend_AlphaKey
	{
		if (resultColor.a < ((224.0f / 255.0f))) 
			discard;
	}
	else 
	{
		if (resultColor.a < ((1.0f / 255.0f))) 
			discard;
	}
	
	float4 colorAndAlpha = gColor;
	
	if (gBlendMode == 0 || gBlendMode == 1) 
		colorAndAlpha.rgb *= IN.color.rgb; // It looks like in order to get correct picture the color from SMODoodadDef should be applied only to opaque submeshes of M2.
	
	colorAndAlpha.a *= gTextureWeight /*TODO: all M2 alpha*/;
	colorAndAlpha.a *= IN.color.a;

	if (gShader == 1)
	{
		const float4 tex1 = DiffuseTexture1.Sample(DiffuseTexture1Sampler, float2(IN.texCoord1.x, 1.0f - IN.texCoord1.y));
		resultColor *= tex1;
	}

	resultColor = MixColorAndTexture(gBlendMode, colorAndAlpha, resultColor);
	//resultColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	DefferedRenderPSOut OUT;
	OUT.Diffuse = resultColor;
	OUT.Specular = float4(0.5f, 0.5f, 0.5f, 1.0f);
	OUT.NormalWS = float4(IN.normal, 0.0f);
	return OUT;
}


float4 MixColorAndTexture(uint BlendMode, float4 _in, float4 tex0)
{
	return _in * tex0;
	
	if (BlendMode == 0) // M2BLEND_OPAQUE
	{
		return float4(_in.rgb * tex0.rgb, _in.a);
	}
	else if (BlendMode == 1) // M2BLEND_ALPHA_KEY
	{
		return float4(_in.rgb * tex0.rgb, _in.a);
	}
	else if (BlendMode == 2) // M2BLEND_ALPHA
	{
		return float4(lerp(tex0.rgb, _in.rgb, _in.a), _in.a);
	}
	else if (BlendMode == 3) // M2BLEND_ADD
	{
		return float4(_in.rgb + tex0.rgb, _in.a + tex0.a);
	}
	else if (BlendMode == 4) // M2BLEND_MOD
	{
		return float4(_in.rgb * tex0.rgb, _in.a * tex0.a);
	}
	else if (BlendMode == 5) // M2BLEND_MOD2X
	{
		return float4(_in.rgb * tex0.rgb * 2.0f, _in.a * tex0.a * 2.0f);
	}
	else if (BlendMode == 10) // M2BLEND_NO_ALPHA_ADD
	{
		return float4(_in.rgb + tex0.rgb, _in.a);
	}
	
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

float4 Test(VertexShaderOutput IN)
{
	float4 tex0 = DiffuseTexture0.Sample(DiffuseTexture0Sampler, IN.texCoord0);
	float4 tex1 = DiffuseTexture1.Sample(DiffuseTexture1Sampler, IN.texCoord1);

	float4 _in  = float4(0.5f, 0.5f, 0.5f, tex0.a);
	float4 _out = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	_in = tex0;
	
	if (gShader == 0)
	{
		//Combiners_Add	
		_out.rgb = _in.rgb + tex0.rgb;	
		_out.a = _in.a + tex0.a;
	}
	else if (gShader == 1)
	{
		//Combiners_Decal
		_out.rgb = lerp(_in.rgb, tex0.rgb, _in.a);	
		_out.a = _in.a;
	}
	else if (gShader == 2)
	{
		//Combiners_Fade	
		_out.rgb = lerp(tex0.rgb, _in.rgb, _in.a);	
		_out.a = _in.a;
	}
	else if (gShader == 3)
	{
		//Combiners_Mod	
		_out.rgb = _in.rgb * tex0.rgb;	
		_out.a = _in.a * tex0.a;
	}
	else if (gShader == 4)
	{
		//Combiners_Mod2x	
		_out.rgb = _in.rgb * tex0.rgb * 2.0;	
		_out.a = _in.a * tex0.a * 2.0;
	}
	else if (gShader == 5)
	{
		//Combiners_Opaque	
		_out.rgb = _in.rgb * tex0.rgb;	
		_out.a = _in.a;
	}
	else if (gShader == 6)
	{
		//Combiners_Add_Add	
		_out.rgb = (_in.rgb + tex0.rgb) + tex1.rgb;	
		_out.a = (_in.a + tex0.a) + tex1.a;
	}
	else if (gShader == 7)
	{
		//Combiners_Add_Mod	
		_out.rgb = (_in.rgb + tex0.rgb) * tex1.rgb;	
		_out.a = (_in.a + tex0.a) * tex1.a;
	}
	else if (gShader == 8)
	{
		//Combiners_Add_Mod2x
		_out.rgb = (_in.rgb + tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = (_in.a + tex0.a) * tex1.a * 2.0;
	}
	else if (gShader == 9)
	{
		//Combiners_Add_Opaque
		_out.rgb = (_in.rgb + tex0.rgb) * tex1.rgb;	
		_out.a = _in.a + tex0.a;
	}
	else if (gShader == 10)
	{
		//Combiners_Mod_Add
		_out.rgb = (_in.rgb * tex0.rgb) + tex1.rgb;	
		_out.a = (_in.a * tex0.a) + tex1.a;
	}
	else if (gShader == 11)
	{
		//Combiners_Mod_AddNA
		_out.rgb = (_in.rgb * tex0.rgb) + tex1.rgb;	
		_out.a = _in.a * tex0.a;
	}
	else if (gShader == 12)
	{
		//Combiners_Mod_Mod
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb;	
		_out.a = (_in.a * tex0.a) * tex1.a;
	}
	else if (gShader == 13)
	{
		//Combiners_Mod_Mod2x	2	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = (_in.a * tex0.a) * tex1.a * 2.0;
	}
	else if (gShader == 14)
	{
		//Combiners_Mod_Mod2xNA	2	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = _in.a * tex0.a;
	}
	else if (gShader == 15)
	{
		//Combiners_Mod_Opaque	2	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb;	
		_out.a = _in.a * tex0.a;
	}
	else if (gShader == 16)
	{
		//Combiners_Mod2x_Add	2	// TODO	// TODO
		_out = float4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (gShader == 17)
	{
		//Combiners_Mod2x_Mod2x		// TODO	// TODO
		_out = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else if (gShader == 18)
	{
		//Combiners_Mod2x_Opaque	// TODO	// TODO
		_out = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else if (gShader == 19)
	{
		//Combiners_Opaque_Add	
		_out.rgb = (_in.rgb * tex0.rgb) + tex1.rgb;	
		_out.a = _in.a + tex1.a;
	}
	else if (gShader == 20)
	{
		//Combiners_Opaque_AddAlpha
		_out.rgb = (_in.rgb * tex0.rgb) + (tex1.rgb * tex1.a);	
		_out.a = _in.a;
	}
	else if (gShader == 21)
	{
		//Combiners_Opaque_AddAlpha_Alpha	
		_out.rgb = (_in.rgb * tex0.rgb) + (tex1.rgb * tex1.a * tex0.a);	
		_out.a = _in.a;
	}
	else if (gShader == 22)
	{
		//Combiners_Opaque_AddNA	
		_out.rgb = (_in.rgb * tex0.rgb) + tex1.rgb;	
		_out.a = _in.a;
	}
	else if (gShader == 23)
	{
		//Combiners_Opaque_Mod	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb;	
		_out.a = _in.a * tex1.a;
	}
	else if (gShader == 24)
	{
		//Combiners_Opaque_Mod2x	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = _in.a * tex1.a * 2.0;
	}
	else if (gShader == 25)
	{
		//Combiners_Opaque_Mod2xNA	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = _in.a;
	}
	else if (gShader == 26)
	{
		//Combiners_Opaque_Mod2xNA_Alpha	
		_out.rgb = (_in.rgb * tex0.rgb) * lerp(tex1.rgb * 2.0, float3(1.0f, 1.0f, 1.0f), tex0.a);	
		_out.a = _in.a;
	}
	else if (gShader == 27)
	{
		//Combiners_Opaque_Opaque	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb;	
		_out.a = _in.a;
	}

	return _out;
}