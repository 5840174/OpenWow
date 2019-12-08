#pragma once

// FORWARD BEGIN
class CMapTile;
// FORWARD END

class ADT_MCNK_Material : public MaterialProxie
{
public:
	ADT_MCNK_Material(std::shared_ptr<IRenderDevice> RenderDevice, const std::weak_ptr<CMapTile> _parentADT);
	virtual ~ADT_MCNK_Material();

	void SetLayersCnt(uint32 value);
	void SetShadowMapExists(uint32 value);

protected:
	void UpdateConstantBuffer() const;

private:
	__declspec(align(16)) struct MaterialProperties
	{
		MaterialProperties()
			: m_ShadowMapExists(0)
		{}
		uint32 m_LayersCnt;
		uint32 m_ShadowMapExists;
		vec2   m_Pad;
		//-------------------------- ( 32 bytes )
	};
	MaterialProperties* m_pProperties;
};