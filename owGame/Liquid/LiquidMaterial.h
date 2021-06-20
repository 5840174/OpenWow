#pragma once

#include "DBC/Tables/DBC_LiquidType.h"

namespace
{
	__declspec(align(16)) struct SLiquidMaterialProperties
	{
		SLiquidMaterialProperties()
			: gColorLight(1.0f, 1.0f, 1.0f)
			, gColorDark(1.0f, 1.0f, 1.0f)
			, gShallowAlpha(1.0f)
			, gDeepAlpha(1.0f)
			, gLiquidType(0)
		{}

		glm::vec3 gColorLight;
		float gShallowAlpha;

		glm::vec3 gColorDark;
		float gDeepAlpha;

		uint32 gLiquidType;
		glm::vec3 __padding;
		//-------------------------- ( 48 bytes )
	};
}

class ZN_API CLiquidMaterial
	: public MaterialProxieT<SLiquidMaterialProperties>
{
public:
	CLiquidMaterial(IRenderDevice& RenderDevice);
	virtual ~CLiquidMaterial();

	// CLiquidMaterial
	void SetShallowAlpha(float value);
	void SetDeepAlpha(float value);
	void SetColorLight(glm::vec3 value);
	void SetColorDark(glm::vec3 value);

	void InitializeTextures(const DBC_LiquidTypeRecord* LiquidTypeRecord);
	void Update();

private:
	IRenderDevice& m_RenderDevice;
	std::vector<std::shared_ptr<ITexture>> m_Textures;
};