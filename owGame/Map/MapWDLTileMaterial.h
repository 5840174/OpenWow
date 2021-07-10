#pragma once

namespace
{
__declspec(align(16)) struct CMapWDLTileMaterialProperties
{
	CMapWDLTileMaterialProperties()
		: DiffuseColor(1, 1, 1, 1)
	{}

	ColorRGBA DiffuseColor;
	//-------------------------- ( 16 bytes )
};
}

class CMapWDLTileMaterial 
	: public MaterialProxieT<CMapWDLTileMaterialProperties>
{
public:
	CMapWDLTileMaterial(IRenderDevice& RenderDevice);
	virtual ~CMapWDLTileMaterial();

	const ColorRGBA& GetDiffuseColor() const;
	void SetDiffuseColor(const ColorRGBA& diffuse);
};