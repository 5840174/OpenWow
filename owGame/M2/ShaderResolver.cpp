#include "stdafx.h"

#ifdef USE_M2_MODELS

#include "M2_Types.h"

#include "ShaderResolver.h"

enum EGxTexOp : uint32
{
	GxTexOp_Mod = 0,
	GxTexOp_Mod2x,
	GxTexOp_Add,
	GxTexOp_PassThru,
	GxTexOp_Decal,
	GxTexOp_Fade

};

/*
0 PS_Combiners_Add	1	out.rgb = in.rgb + tex0.rgb;	out.a = in.a + tex0.a;
1 PS_Combiners_Decal	1	out.rgb = mix(in.rgb, tex0.rgb, in.a);	out.a = in.a;
2 PS_Combiners_Fade	1	out.rgb = mix(tex0.rgb, in.rgb, in.a);	out.a = in.a;
3 PS_Combiners_Mod	1	out.rgb = in.rgb * tex0.rgb;	out.a = in.a * tex0.a;
4 PS_Combiners_Mod2x	1	out.rgb = in.rgb * tex0.rgb * 2.0;	out.a = in.a * tex0.a * 2.0;
5 PS_Combiners_Opaque	1	out.rgb = in.rgb * tex0.rgb;	out.a = in.a;
6 PS_Combiners_Add_Add	2	out.rgb = (in.rgb + tex0.rgb) + tex1.rgb;	out.a = (in.a + tex0.a) + tex1.a;
7 PS_Combiners_Add_Mod	2	out.rgb = (in.rgb + tex0.rgb) * tex1.rgb;	out.a = (in.a + tex0.a) * tex1.a;
8 PS_Combiners_Add_Mod2x	2	out.rgb = (in.rgb + tex0.rgb) * tex1.rgb * 2.0;	out.a = (in.a + tex0.a) * tex1.a * 2.0;
9 PS_Combiners_Add_Opaque	2	out.rgb = (in.rgb + tex0.rgb) * tex1.rgb;	out.a = in.a + tex0.a;
10 PS_Combiners_Mod_Add	2	out.rgb = (in.rgb * tex0.rgb) + tex1.rgb;	out.a = (in.a * tex0.a) + tex1.a;
11 PS_Combiners_Mod_AddNA	2	out.rgb = (in.rgb * tex0.rgb) + tex1.rgb;	out.a = in.a * tex0.a;
12 PS_Combiners_Mod_Mod	2	out.rgb = (in.rgb * tex0.rgb) * tex1.rgb;	out.a = (in.a * tex0.a) * tex1.a;
13 PS_Combiners_Mod_Mod2x	2	out.rgb = (in.rgb * tex0.rgb) * tex1.rgb * 2.0;	out.a = (in.a * tex0.a) * tex1.a * 2.0;
14 PS_Combiners_Mod_Mod2xNA	2	out.rgb = (in.rgb * tex0.rgb) * tex1.rgb * 2.0;	out.a = in.a * tex0.a;
15 PS_Combiners_Mod_Opaque	2	out.rgb = (in.rgb * tex0.rgb) * tex1.rgb;	out.a = in.a * tex0.a;
16 PS_Combiners_Mod2x_Add	2	// TODO	// TODO
17 PS_Combiners_Mod2x_Mod2x	2	// TODO	// TODO
18 PS_Combiners_Mod2x_Opaque	2	// TODO	// TODO
19 PS_Combiners_Opaque_Add	2	out.rgb = (in.rgb * tex0.rgb) + tex1.rgb;	out.a = in.a + tex1.a;
20 PS_Combiners_Opaque_AddAlpha	2	out.rgb = (in.rgb * tex0.rgb) + (tex1.rgb * tex1.a);	out.a = in.a;
21 PS_Combiners_Opaque_AddAlpha_Alpha	2	out.rgb = (in.rgb * tex0.rgb) + (tex1.rgb * tex1.a * tex0.a);	out.a = in.a;
22 PS_Combiners_Opaque_AddNA	2	out.rgb = (in.rgb * tex0.rgb) + tex1.rgb;	out.a = in.a;
23 PS_Combiners_Opaque_Mod	2	out.rgb = (in.rgb * tex0.rgb) * tex1.rgb;	out.a = in.a * tex1.a;
24 PS_Combiners_Opaque_Mod2x	2	out.rgb = (in.rgb * tex0.rgb) * tex1.rgb * 2.0;	out.a = in.a * tex1.a * 2.0;
25 PS_Combiners_Opaque_Mod2xNA	2	out.rgb = (in.rgb * tex0.rgb) * tex1.rgb * 2.0;	out.a = in.a;
26 PS_Combiners_Opaque_Mod2xNA_Alpha	2	out.rgb = (in.rgb * tex0.rgb) * mix(tex1.rgb * 2.0, vec3(1.0), tex0.a);	out.a = in.a;
27 PS_Combiners_Opaque_Opaque	2	out.rgb = (in.rgb * tex0.rgb) * tex1.rgb;	out.a = in.a;
*/

uint16 arr[] =
{
	/*PS_Combiners_Opaque = */5,
	/*PS_Combiners_Mod = */3,
	/*PS_Combiners_Opaque_Mod = */23,
	/*PS_Combiners_Opaque_Mod2x = */24,
	/*PS_Combiners_Opaque_Mod2xNA = */25,
	/*PS_Combiners_Opaque_Opaque = */27,
	/*PS_Combiners_Mod_Mod = */12,
	/*PS_Combiners_Mod_Mod2x = */13,
	/*PS_Combiners_Mod_Add = */10,
	/*PS_Combiners_Mod_Mod2xNA = */14,
	/*PS_Combiners_Mod_AddNA = */11,
	/*PS_Combiners_Mod_Opaque = */15,
	/*PS_Combiners_Opaque_Mod2xNA_Alpha = */26,
	/*PS_Combiners_Opaque_AddAlpha = */20,
	/*PS_Combiners_Opaque_AddAlpha_Alpha = */21,
	/*PS_Combiners_Opaque_Mod2xNA_Alpha_Add,*/
	/*PS_Combiners_Mod_AddAlpha,*/
	/*PS_Combiners_Mod_AddAlpha_Alpha,*/
	/*PS_Combiners_Opaque_Alpha_Alpha,*/
	/*PS_Combiners_Opaque_Mod2xNA_Alpha_3s,*/
	/*PS_Combiners_Opaque_AddAlpha_Wgt,*/
	/*PS_Combiners_Mod_Add_Alpha,*/
	/*PS_Combiners_Opaque_ModNA_Alpha,*/
	/*PS_Combiners_Mod_AddAlpha_Wgt,*/
	/*PS_Combiners_Opaque_Mod_Add_Wgt,*/
	/*PS_Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha,*/
	/*PS_Combiners_Mod_Dual_Crossfade,*/
	/*PS_Combiners_Opaque_Mod2xNA_Alpha_Alpha,*/
	/*PS_Combiners_Mod_Masked_Dual_Crossfade,*/
	/*PS_Combiners_Opaque_Alpha,*/
};

enum modelVertexShaders
{
	VS_Diffuse_T1,
	VS_Diffuse_Env,
	VS_Diffuse_T1_T2,
	VS_Diffuse_T1_Env,
	VS_Diffuse_Env_T1,
	VS_Diffuse_Env_Env,
	VS_Diffuse_T1_Env_T1,
	VS_Diffuse_T1_T1,
	VS_Diffuse_T1_T1_T1,
	VS_Diffuse_EdgeFade_T1,
	VS_Diffuse_T2,
	VS_Diffuse_T1_Env_T2,
	VS_Diffuse_EdgeFade_T1_T2,
	VS_Diffuse_T1_T1_T1_T2,
	VS_Diffuse_EdgeFade_Env,
	VS_Diffuse_T1_T2_T1,
};

const char* s_modelVertexShaders[16] =
{
	"Diffuse_T1",
	"Diffuse_Env",
	"Diffuse_T1_T2",
	"Diffuse_T1_Env",
	"Diffuse_Env_T1",
	"Diffuse_Env_Env",
	"Diffuse_T1_Env_T1",
	"Diffuse_T1_T1",
	"Diffuse_T1_T1_T1",
	"Diffuse_EdgeFade_T1",
	"Diffuse_T2",
	"Diffuse_T1_Env_T2",
	"Diffuse_EdgeFade_T1_T2",
	"Diffuse_T1_T1_T1_T2",
	"Diffuse_EdgeFade_Env",
	"Diffuse_T1_T2_T1",
};




enum modelPixelShaders
{
	PS_Combiners_Opaque ,
	PS_Combiners_Mod ,
	PS_Combiners_Opaque_Mod ,
	PS_Combiners_Opaque_Mod2x ,
	PS_Combiners_Opaque_Mod2xNA ,
	PS_Combiners_Opaque_Opaque,
	PS_Combiners_Mod_Mod,
	PS_Combiners_Mod_Mod2x,
	PS_Combiners_Mod_Add,
	PS_Combiners_Mod_Mod2xNA,
	PS_Combiners_Mod_AddNA,
	PS_Combiners_Mod_Opaque,
	PS_Combiners_Opaque_Mod2xNA_Alpha,
	PS_Combiners_Opaque_AddAlpha,
	PS_Combiners_Opaque_AddAlpha_Alpha,
	PS_Combiners_Opaque_Mod2xNA_Alpha_Add,
	PS_Combiners_Mod_AddAlpha,
	PS_Combiners_Mod_AddAlpha_Alpha,
	PS_Combiners_Opaque_Alpha_Alpha,
	PS_Combiners_Opaque_Mod2xNA_Alpha_3s,
	PS_Combiners_Opaque_AddAlpha_Wgt,
	PS_Combiners_Mod_Add_Alpha,
	PS_Combiners_Opaque_ModNA_Alpha,
	PS_Combiners_Mod_AddAlpha_Wgt,
	PS_Combiners_Opaque_Mod_Add_Wgt,
	PS_Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha,
	PS_Combiners_Mod_Dual_Crossfade,
	PS_Combiners_Opaque_Mod2xNA_Alpha_Alpha,
	PS_Combiners_Mod_Masked_Dual_Crossfade,
	PS_Combiners_Opaque_Alpha,
	PS_Guild,
	PS_Guild_NoBorder,
	PS_Guild_Opaque,
	PS_Combiners_Mod_Depth,
	PS_Illum,
	PS_Combiners_Mod_Mod_Mod_Const,
};
const char* s_modelPixelShaders[36] =
{
	"Combiners_Opaque",
	"Combiners_Mod",
	"Combiners_Opaque_Mod",
	"Combiners_Opaque_Mod2x",
	"Combiners_Opaque_Mod2xNA",
	"Combiners_Opaque_Opaque",
	"Combiners_Mod_Mod",
	"Combiners_Mod_Mod2x",
	"Combiners_Mod_Add",
	"Combiners_Mod_Mod2xNA",
	"Combiners_Mod_AddNA",
	"Combiners_Mod_Opaque",
	"Combiners_Opaque_Mod2xNA_Alpha",
	"Combiners_Opaque_AddAlpha",
	"Combiners_Opaque_AddAlpha_Alpha",
	"Combiners_Opaque_Mod2xNA_Alpha_Add",
	"Combiners_Mod_AddAlpha",
	"Combiners_Mod_AddAlpha_Alpha",
	"Combiners_Opaque_Alpha_Alpha",
	"Combiners_Opaque_Mod2xNA_Alpha_3s",
	"Combiners_Opaque_AddAlpha_Wgt",
	"Combiners_Mod_Add_Alpha",
	"Combiners_Opaque_ModNA_Alpha",
	"Combiners_Mod_AddAlpha_Wgt",
	"Combiners_Opaque_Mod_Add_Wgt",
	"Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha",
	"Combiners_Mod_Dual_Crossfade",
	"Combiners_Opaque_Mod2xNA_Alpha_Alpha",
	"Combiners_Mod_Masked_Dual_Crossfade",
	"Combiners_Opaque_Alpha",
	"Guild",
	"Guild_NoBorder",
	"Guild_Opaque",
	"Combiners_Mod_Depth",
	"Illum",
	"Combiners_Mod_Mod_Mod_Const",
};

enum modelHullShaders
{
	HS_T1,
	HS_T1_T2,
	HS_T1_T2_T3,
	HS_T1_T2_T3_T4,
};
const char* s_modelHullShaders[16] =
{
	"T1",
	"T1_T2",
	"T1_T2_T3",
	"T1_T2_T3_T4",
};

enum modelDomainShaders
{
	DS_T1,
	DS_T1_T2,
	DS_T1_T2_T3,
	DS_T1_T2_T3_T4,
};
const char* s_modelDomainShaders[16] =
{
	"T1",
	"T1_T2",
	"T1_T2_T3",
	"T1_T2_T3_T4",
};


#define NUM_M2SHADERS 30

struct
{
	uint32 pixel;
	uint32 vertex;
	uint32 hull;
	uint32 domain;
	uint32 ff_colorOp;
	uint32 ff_alphaOp;
}
s_modelShaderEffect[100] =
{
	{ PS_Combiners_Opaque_Mod2xNA_Alpha,           VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Opaque_AddAlpha,                VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Opaque_AddAlpha_Alpha,          VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Opaque_Mod2xNA_Alpha_Add,       VS_Diffuse_T1_Env_T1,      HS_T1_T2_T3,    DS_T1_T2_T3,    0, 3 },
	{ PS_Combiners_Mod_AddAlpha,                   VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 0 },
	{ PS_Combiners_Opaque_AddAlpha,                VS_Diffuse_T1_T1,          HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Mod_AddAlpha,                   VS_Diffuse_T1_T1,          HS_T1_T2,       DS_T1_T2,       0, 0 },
	{ PS_Combiners_Mod_AddAlpha_Alpha,             VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 0 },
	{ PS_Combiners_Opaque_Alpha_Alpha,             VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Opaque_Mod2xNA_Alpha_3s,        VS_Diffuse_T1_Env_T1,      HS_T1_T2_T3,    DS_T1_T2_T3,    0, 3 },
	{ PS_Combiners_Opaque_AddAlpha_Wgt,            VS_Diffuse_T1_T1,          HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Mod_Add_Alpha,                  VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 0 },
	{ PS_Combiners_Opaque_ModNA_Alpha,             VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Mod_AddAlpha_Wgt,               VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Mod_AddAlpha_Wgt,               VS_Diffuse_T1_T1,          HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Opaque_AddAlpha_Wgt,            VS_Diffuse_T1_T2,          HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Opaque_Mod_Add_Wgt,             VS_Diffuse_T1_Env,         HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha, VS_Diffuse_T1_Env_T1,      HS_T1_T2_T3,    DS_T1_T2_T3,    0, 3 },
	{ PS_Combiners_Mod_Dual_Crossfade,             VS_Diffuse_T1_T1_T1,       HS_T1_T2_T3,    DS_T1_T2_T3,    0, 0 },
	{ PS_Combiners_Mod_Depth,                      VS_Diffuse_EdgeFade_T1,    HS_T1,          DS_T1,          0, 0 },
	{ PS_Combiners_Mod_AddAlpha_Alpha,             VS_Diffuse_T1_Env_T2,      HS_T1_T2_T3,    DS_T1_T2_T3,    0, 3 },
	{ PS_Combiners_Mod_Mod,                        VS_Diffuse_EdgeFade_T1_T2, HS_T1_T2,       DS_T1_T2,       0, 0 },
	{ PS_Combiners_Mod_Masked_Dual_Crossfade,      VS_Diffuse_T1_T1_T1_T2,    HS_T1_T2_T3_T4, DS_T1_T2_T3_T4, 0, 0 },
	{ PS_Combiners_Opaque_Alpha,                   VS_Diffuse_T1_T1,          HS_T1_T2,       DS_T1_T2,       0, 3 },
	{ PS_Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha, VS_Diffuse_T1_Env_T2,      HS_T1_T2_T3,    DS_T1_T2_T3,    0, 3 },
	{ PS_Combiners_Mod_Depth,                      VS_Diffuse_EdgeFade_Env,   HS_T1,          DS_T1,          0, 0 },
	{ PS_Guild,                                    VS_Diffuse_T1_T2_T1,       HS_T1_T2_T3,    DS_T1_T2,       0, 0 },
	{ PS_Guild_NoBorder,                           VS_Diffuse_T1_T2,          HS_T1_T2,       DS_T1_T2_T3,    0, 0 },
	{ PS_Guild_Opaque,                             VS_Diffuse_T1_T2_T1,       HS_T1_T2_T3,    DS_T1_T2,       0, 0 },
	{ PS_Illum,                                    VS_Diffuse_T1_T1,          HS_T1_T2,       DS_T1_T2,       0, 0 },
};

uint32 M2GetPixelShaderID(uint32 op_count, uint16 shader_id)
{
	if (shader_id & 0x8000)
	{
		uint16 const shaderID(shader_id & (~0x8000));
		_ASSERT(shaderID < NUM_M2SHADERS);
		return s_modelShaderEffect[shaderID].pixel;
	}
	else
	{
		if (op_count == 1)
		{
			return (shader_id & 0x70) ? PS_Combiners_Mod : PS_Combiners_Opaque;
		}
		else
		{
			const uint32 lower(shader_id & 7);
			if (shader_id & 0x70)
			{
				return lower == 0 ? PS_Combiners_Mod_Opaque
					: lower == 3 ? PS_Combiners_Mod_Add
					: lower == 4 ? PS_Combiners_Mod_Mod2x
					: lower == 6 ? PS_Combiners_Mod_Mod2xNA
					: lower == 7 ? PS_Combiners_Mod_AddNA
					: PS_Combiners_Mod_Mod;
			}
			else
			{
				return lower == 0 ? PS_Combiners_Opaque_Opaque
					: lower == 3 ? PS_Combiners_Opaque_AddAlpha
					: lower == 4 ? PS_Combiners_Opaque_Mod2x
					: lower == 6 ? PS_Combiners_Opaque_Mod2xNA
					: lower == 7 ? PS_Combiners_Opaque_AddAlpha
					: PS_Combiners_Opaque_Mod;
			}
		}
	}
}
uint32 M2GetVertexShaderID(uint32 op_count, uint16 shader_id)
{
	if (shader_id & 0x8000)
	{
		uint16 const shaderID(shader_id & (~0x8000));
		_ASSERT(shaderID < NUM_M2SHADERS);
		return s_modelShaderEffect[shaderID].vertex;
	}
	else
	{
		if (op_count == 1)
		{
			return shader_id & 0x80 ? VS_Diffuse_Env
				: shader_id & 0x4000 ? VS_Diffuse_T2
				: VS_Diffuse_T1;
		}
		else
		{
			if (shader_id & 0x80)
			{
				return shader_id & 0x8 ? VS_Diffuse_Env_Env
					: VS_Diffuse_Env_T1;
			}
			else
			{
				return shader_id & 0x8 ? VS_Diffuse_T1_Env
					: shader_id & 0x4000 ? VS_Diffuse_T1_T2
					: VS_Diffuse_T1_T1;
			}
		}
	}
}
uint32 M2GetHullShaderID(uint32 op_count, uint16 shader_id)
{
	if (shader_id & 0x8000)
	{
		uint16 const shaderID(shader_id & (~0x8000));
		_ASSERT(shaderID < NUM_M2SHADERS);
		return s_modelShaderEffect[shaderID].hull;
	}
	else
	{
		return op_count == 1 ? HS_T1 : HS_T1_T2;
	}
}
uint32 M2GetDomainShaderID(uint32 op_count, uint16 shader_id)
{
	if (shader_id & 0x8000)
	{
		uint16 const shaderID(shader_id & (~0x8000));
		_ASSERT(shaderID < NUM_M2SHADERS);
		return s_modelShaderEffect[shaderID].domain;
	}
	else
	{
		return op_count == 1 ? DS_T1 : DS_T1_T2;
	}
}
void M2GetFixedFunctionFallback(uint16 shader_id, EGxTexOp* colorOp, EGxTexOp* alphaOp)
{
	if (shader_id & 0x8000)
	{
		uint16 const shaderID(shader_id & (~0x8000));
		_ASSERT(shaderID < NUM_M2SHADERS);
		*colorOp = EGxTexOp(s_modelShaderEffect[shaderID].ff_colorOp);
		*alphaOp = EGxTexOp(s_modelShaderEffect[shaderID].ff_alphaOp);
	}
	else
	{
		*colorOp = EGxTexOp(0);
		*alphaOp = EGxTexOp(shader_id & 0x70 ? 0 : 3);
	}
}
void M2GetCombinerOps(uint16 shader_id, uint32 op_count, EGxTexOp* colorOp, EGxTexOp* alphaOp)
{
	int helper[2] = { (shader_id >> 4) & 7, shader_id & 7 };
	for (int i = 0; i < op_count; ++i)
	{
		//! \todo Add enum.
		static const uint32 alphaOpTable[] = { 3, 0, 3, 2, 1, 3, 3, 3 };
		static const uint32 colorOpTable[] = { 0, 0, 4, 2, 1, 5, 1, 2 };
		(colorOp[i]) = EGxTexOp(colorOpTable[helper[i]]);
		(alphaOp[i]) = EGxTexOp(alphaOpTable[helper[i]]);
	}
}
const char* M2GetPixelShaderName(uint32 op_count, uint16 shader_id)
{
	uint32 pixelShaderID(M2GetPixelShaderID(op_count, shader_id));
	//array_size_check(pixelShaderID, s_modelPixelShaders);
	return s_modelPixelShaders[pixelShaderID];
}
const char* M2GetVertexShaderName(uint32 op_count, uint16 shader_id)
{
	uint32 vertexShaderID(M2GetVertexShaderID(op_count, shader_id));
	//array_size_check(vertexShaderID, s_modelVertexShaders);
	return s_modelVertexShaders[vertexShaderID];
}
const char* M2GetHullShaderName(uint32 op_count, uint16 shader_id)
{
	uint32 hullShaderID(M2GetHullShaderID(op_count, shader_id));
	//array_size_check(hullShaderID, s_modelHullShaders);
	return s_modelHullShaders[hullShaderID];
}
const char* M2GetDomainShaderName(uint32 op_count, uint16 shader_id)
{
	uint32 domainShaderID(M2GetDomainShaderID(op_count, shader_id));
	//array_size_check(domainShaderID, s_modelDomainShaders);
	return s_modelDomainShaders[domainShaderID];
}

void* GetEffect(SM2_SkinBatch* batch)
{
	const char* vertex_shader_name(M2GetVertexShaderName(batch->textureCount, batch->shader_id));
	const char* pixel_shader_name(M2GetPixelShaderName(batch->textureCount, batch->shader_id));

	char hull_shader_name_prefixed[0x100];
	hull_shader_name_prefixed[0] = 0;

	char domain_shader_name_prefixed[0x100];
	domain_shader_name_prefixed[0] = 0;

	/*if (CShaderEffect::TesselationEnabled())
	{
		sprintf_s(hull_shader_name_prefixed, 0x100u, "Model2_%s", M2GetHullShaderName(batch->textureCount, batch->shader_id));
		sprintf_s(domain_shader_name_prefixed, 0x100u, "Model2_%s", M2GetDomainShaderName(batch->textureCount, batch->shader_id));
	}
	else if (CShaderEffect::DisplacementEnabled())
	{
		sprintf_s(hull_shader_name_prefixed, 0x100u, "Model2Displ_%s", M2GetHullShaderName(batch->textureCount, batch->shader_id));
		sprintf_s(domain_shader_name_prefixed, 0x100u, "Model2Displ_%s", M2GetDomainShaderName(batch->textureCount, batch->shader_id));
	}*/

	// assemble effect name and look in cache

	char effect_name[0x100];
	if (batch->shader_id & 0x8000)
	{
		sprintf_s(effect_name, 0x100u, "M2Effect %d", batch->shader_id & (~0x8000));
	}
	else
	{
		strcpy(effect_name, vertex_shader_name);
		strcat(effect_name, pixel_shader_name);
	}

	if (pixel_shader_name == "Combiners_Opaque")
		return nullptr;

	Log::Info("Effect name is [%s]", pixel_shader_name);

	/*CShaderEffect* effect(CShaderEffectManager::GetEffect(effect_name));
	if (effect)
	{
		effect->AddRef();
		return effect;
	}*/

	// create shader and initialize

	//effect = CShaderEffectManager::CreateEffect(effect_name);
	//effect->InitEffect(vertex_shader_name, hull_shader_name_prefixed, domain_shader_name_prefixed, pixel_shader_name);

	if (batch->shader_id < 0)
	{
		EGxTexOp colorOp;
		EGxTexOp alphaOp;
		M2GetFixedFunctionFallback(batch->shader_id, &colorOp, &alphaOp);
		//effect->InitFixedFuncPass(&colorOp, &alphaOp, 1);
	}
	else
	{
		EGxTexOp colorOps[2];
		EGxTexOp alphaOps[2];
		M2GetCombinerOps(batch->shader_id, batch->textureCount, colorOps, alphaOps);
		//effect->InitFixedFuncPass(colorOps, alphaOps, batch->textureCount);
	}

	//_ASSERT(effect);
	return nullptr;
}

int32 GetPixel(const SM2_SkinBatch& batch)
{
	uint32 index = M2GetPixelShaderID(batch.textureCount, batch.shader_id);
	_ASSERT(index < 15);
	return arr[index];
}

#endif
