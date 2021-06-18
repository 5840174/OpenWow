#pragma once

namespace
{
__declspec(align(16)) struct CMapWDLTileMaterialProperties
{
	CMapWDLTileMaterialProperties()
		: m_DiffuseColor(1, 1, 1, 1)
	{}
	glm::vec4   m_DiffuseColor;
	//-------------------------- ( 32 bytes )
};
}

class CMapWDLTileMaterial 
	: public MaterialProxieT<CMapWDLTileMaterialProperties>
{
public:
	CMapWDLTileMaterial(IRenderDevice& RenderDevice);
	virtual ~CMapWDLTileMaterial();

	const glm::vec4& GetDiffuseColor() const;
	void SetDiffuseColor(const glm::vec4& diffuse);	
};