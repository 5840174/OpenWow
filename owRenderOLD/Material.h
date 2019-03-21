#pragma once

class Material
{
public:
	Material();

	void Set() const;
	void Restore();

public:
	void SetDiffuseTexture(std::shared_ptr<Texture> _DiffuseTexture)
	{
		m_DiffuseTexture = _DiffuseTexture;
	}

	void SetSpecularTexture(std::shared_ptr<Texture> _SpecularTexture)
	{
		m_SpecularTexture = _SpecularTexture;
	}
	
	void SetRenderState(bool _IsTwoSided = false, bool _DepthWrite = true, bool _DepthTest = true)
	{
		m_IsTwoSided = _IsTwoSided;
		m_DepthWrite = _DepthWrite;
		m_DepthTest = _DepthTest;
	}

	void SetBlendEGxBlendIndex(int _BlendEGxBlendIndex)
	{
		m_BlendEGxBlendIndex = _BlendEGxBlendIndex;
	}

	void SetBlendState(bool _BlendEnabled = false, R_BlendFunc _Blend_SrcFunc = R_BlendFunc::BS_BLEND_SRC_ALPHA, R_BlendFunc _Blend_DstFunc = R_BlendFunc::BS_BLEND_INV_SRC_ALPHA)
	{
		m_BlendEnabled = _BlendEnabled;
		m_Blend_SrcFunc = _Blend_SrcFunc;
		m_Blend_DstFunc = _Blend_DstFunc;
	}

private:
	// Colors
	vec4 m_AmbientColor; 
	vec4 m_DiffuseColor; 
	vec4 m_SpecularColor;

	// Textures
	std::shared_ptr<Texture> m_DiffuseTexture;  
	std::shared_ptr<Texture> m_SpecularTexture;

	// Render Modes
	bool m_IsTwoSided;
	bool m_DepthWrite;
	bool m_DepthTest;

	// Blend
	int m_BlendEGxBlendIndex;
	bool m_BlendEnabled;
	R_BlendFunc m_Blend_SrcFunc;
	R_BlendFunc m_Blend_DstFunc;

	CGroupQuality& m_QualitySettings;

public:
	static const uint32 C_DiffuseTextureIndex = 6;
	static const uint32 C_SpecularTextureIndex = 10;
};