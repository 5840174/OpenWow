#pragma once

// FORWARD BEGIN
class CMapTile;
// FORWARD END

class ADT_MCNK_Material 
	: public MaterialProxie
{
public:
	ADT_MCNK_Material(IRenderDevice& RenderDevice);
	virtual ~ADT_MCNK_Material();

	void SetLayersCnt(uint32 value);
	void SetShadowMapExists(uint32 value);

protected:
	void UpdateConstantBuffer() const;

private:
	__declspec(align(16)) struct MaterialProperties
	{
		MaterialProperties()
			: LayersCnt(1)
			, ShadowMapExists(0)
		{}
		uint32 LayersCnt;
		uint32 ShadowMapExists;
		glm::vec2 Pad;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties* m_pProperties;
};