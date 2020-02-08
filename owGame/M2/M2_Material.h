#pragma once

#include "M2_Part_Texture.h"

class M2_Material : public MaterialProxie
{
public:
	M2_Material(IRenderDevice& RenderDevice, std::vector<std::weak_ptr<const CM2_Part_Texture>> m2Textures);
	virtual ~M2_Material();

	void SetAnimated(bool value);
	void SetMaxInfluences(uint32 value);
	void SetBlendMode(uint32 value);
	void SetNewShader(uint32 value);

	void SetTextureWeightEnable(bool value);
	void SetTextureWeight(float value);

	void SetTextureAnimEnable(bool value);
	void SetTextureAnimMatrix(cmat4 value);

	void SetColorEnable(bool value);
	void SetColor(vec4 value);

	void SetBones(const std::vector<mat4>& bones);
	
protected:
	void UpdateConstantBuffer() const override;

private:
	__declspec(align(16)) struct MaterialProperties
	{
		MaterialProperties()
			: gIsAnimated(0)
			, gColorEnable(0)
			, gTextureWeightEnable(0)
			, gTextureAnimEnable(0)

			, gBonesMaxInfluences(0)
			, gBlendMode(0)
			, gShader(0)
			, gTextureWeight(0.0f)
		{}
		uint32 gIsAnimated;
		uint32 gColorEnable;
		uint32 gTextureWeightEnable;
		uint32 gTextureAnimEnable;

		uint32 gBonesMaxInfluences;
		uint32 gBlendMode;
		uint32 gShader;
		float  gTextureWeight;

		mat4 gTextureAnimMatrix;

		vec4 gColor;

		mat4 Bones[200];
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties* m_pProperties;
};