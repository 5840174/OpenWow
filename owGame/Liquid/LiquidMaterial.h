#pragma once

namespace
{
	__declspec(align(16)) struct SLiquidMaterialProperties
	{
		SLiquidMaterialProperties()
			: gLiquidType(0)
		{}

		uint32 gLiquidType;
		glm::vec3 __padding;
		//-------------------------- ( 16 bytes )
	};
}

class ZN_API CLiquidMaterial
	: public MaterialProxieT<SLiquidMaterialProperties>
{
public:
	CLiquidMaterial(IRenderDevice& RenderDevice);
	virtual ~CLiquidMaterial();

	// CLiquidMaterial
	void InitializeTextures(const DBC_LiquidTypeRecord* LiquidTypeRecord);
	void Update();

private:
	IRenderDevice& m_RenderDevice;
	std::vector<std::shared_ptr<ITexture>> m_Textures;
};