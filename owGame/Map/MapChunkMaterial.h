#pragma once

// FORWARD BEGIN
class CMapTile;
// FORWARD END

namespace
{
__declspec(align(16)) struct ADT_MCNK_MaterialMaterialProperties
{
	ADT_MCNK_MaterialMaterialProperties()
		: LayersCnt(1)
		, ShadowMapExists(0)
	{}
	uint32 LayersCnt;
	uint32 ShadowMapExists;
	glm::vec2 Pad;
	//-------------------------- ( 32 bytes )
};
}

class ADT_MCNK_Material 
	: public MaterialProxieT<ADT_MCNK_MaterialMaterialProperties>
{
public:
	ADT_MCNK_Material(IRenderDevice& RenderDevice);
	virtual ~ADT_MCNK_Material();

	void SetLayersCnt(uint32 value);
	void SetShadowMapExists(uint32 value);
};