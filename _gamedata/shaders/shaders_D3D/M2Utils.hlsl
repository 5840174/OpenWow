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

float4 Test(uint ShaderType, float4 InputColor, Texture2D Texture0, sampler Sampler0, float2 TexCoord0, Texture2D Texture1, sampler Sampler1, float2 TexCoord1)
{
	float4 tex0 = Texture0.Sample(Sampler0, float2(TexCoord0.x, 1.0f - TexCoord0.y));
	float4 tex1 = Texture1.Sample(Sampler1, float2(TexCoord1.x, 1.0f - TexCoord1.y));

	float4 _in  = InputColor;
	float4 _out = float4(1.0f, 0.0f, 0.0f, 1.0f);
		
	if (ShaderType == 0)
	{
		//Combiners_Add	
		_out.rgb = _in.rgb + tex0.rgb;	
		_out.a = _in.a + tex0.a;
	}
	else if (ShaderType == 1)
	{
		//Combiners_Decal
		_out.rgb = lerp(_in.rgb, tex0.rgb, _in.a);	
		_out.a = _in.a;
	}
	else if (ShaderType == 2)
	{
		//Combiners_Fade	
		_out.rgb = lerp(tex0.rgb, _in.rgb, _in.a);	
		_out.a = _in.a;
	}
	else if (ShaderType == 3)
	{
		//Combiners_Mod	
		_out.rgb = _in.rgb * tex0.rgb;	
		_out.a = _in.a * tex0.a;
	}
	else if (ShaderType == 4)
	{
		//Combiners_Mod2x	
		_out.rgb = _in.rgb * tex0.rgb * 2.0;	
		_out.a = _in.a * tex0.a * 2.0;
	}
	else if (ShaderType == 5)
	{
		//Combiners_Opaque	
		_out.rgb = _in.rgb * tex0.rgb;	
		_out.a = _in.a;
	}
	else if (ShaderType == 6)
	{
		//Combiners_Add_Add	
		_out.rgb = (_in.rgb + tex0.rgb) + tex1.rgb;	
		_out.a = (_in.a + tex0.a) + tex1.a;
	}
	else if (ShaderType == 7)
	{
		//Combiners_Add_Mod	
		_out.rgb = (_in.rgb + tex0.rgb) * tex1.rgb;	
		_out.a = (_in.a + tex0.a) * tex1.a;
	}
	else if (ShaderType == 8)
	{
		//Combiners_Add_Mod2x
		_out.rgb = (_in.rgb + tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = (_in.a + tex0.a) * tex1.a * 2.0;
	}
	else if (ShaderType == 9)
	{
		//Combiners_Add_Opaque
		_out.rgb = (_in.rgb + tex0.rgb) * tex1.rgb;	
		_out.a = _in.a + tex0.a;
	}
	else if (ShaderType == 10)
	{
		//Combiners_Mod_Add
		_out.rgb = (_in.rgb * tex0.rgb) + tex1.rgb;	
		_out.a = (_in.a * tex0.a) + tex1.a;
	}
	else if (ShaderType == 11)
	{
		//Combiners_Mod_AddNA
		_out.rgb = (_in.rgb * tex0.rgb) + tex1.rgb;	
		_out.a = _in.a * tex0.a;
	}
	else if (ShaderType == 12)
	{
		//Combiners_Mod_Mod
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb;	
		_out.a = (_in.a * tex0.a) * tex1.a;
	}
	else if (ShaderType == 13)
	{
		//Combiners_Mod_Mod2x	2	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = (_in.a * tex0.a) * tex1.a * 2.0;
	}
	else if (ShaderType == 14)
	{
		//Combiners_Mod_Mod2xNA	2	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = _in.a * tex0.a;
	}
	else if (ShaderType == 15)
	{
		//Combiners_Mod_Opaque	2	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb;	
		_out.a = _in.a * tex0.a;
	}
	else if (ShaderType == 16)
	{
		//Combiners_Mod2x_Add	2	// TODO	// TODO
		_out = float4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (ShaderType == 17)
	{
		//Combiners_Mod2x_Mod2x		// TODO	// TODO
		_out = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else if (ShaderType == 18)
	{
		//Combiners_Mod2x_Opaque	// TODO	// TODO
		_out = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else if (ShaderType == 19)
	{
		//Combiners_Opaque_Add	
		_out.rgb = (_in.rgb * tex0.rgb) + tex1.rgb;	
		_out.a = _in.a + tex1.a;
	}
	else if (ShaderType == 20)
	{
		//Combiners_Opaque_AddAlpha
		_out.rgb = (_in.rgb * tex0.rgb) + (tex1.rgb * tex1.a);	
		_out.a = _in.a;
	}
	else if (ShaderType == 21)
	{
		//Combiners_Opaque_AddAlpha_Alpha	
		_out.rgb = (_in.rgb * tex0.rgb) + (tex1.rgb * tex1.a * tex0.a);	
		_out.a = _in.a;
	}
	else if (ShaderType == 22)
	{
		//Combiners_Opaque_AddNA	
		_out.rgb = (_in.rgb * tex0.rgb) + tex1.rgb;	
		_out.a = _in.a;
	}
	else if (ShaderType == 23)
	{
		//Combiners_Opaque_Mod	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb;	
		_out.a = _in.a * tex1.a;
	}
	else if (ShaderType == 24)
	{
		//Combiners_Opaque_Mod2x	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = _in.a * tex1.a * 2.0;
	}
	else if (ShaderType == 25)
	{
		//Combiners_Opaque_Mod2xNA	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb * 2.0;	
		_out.a = _in.a;
	}
	else if (ShaderType == 26)
	{
		//Combiners_Opaque_Mod2xNA_Alpha	
		_out.rgb = (_in.rgb * tex0.rgb) * lerp(tex1.rgb * 2.0, float3(1.0f, 1.0f, 1.0f), tex0.a);	
		_out.a = _in.a;
	}
	else if (ShaderType == 27)
	{
		//Combiners_Opaque_Opaque	
		_out.rgb = (_in.rgb * tex0.rgb) * tex1.rgb;	
		_out.a = _in.a;
	}

	return _out;
}