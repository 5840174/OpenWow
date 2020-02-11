#include "stdafx.h"

// Include
#include "MapTile.h"

// General
#include "MapChunkMaterial.h"


ADT_MCNK_Material::ADT_MCNK_Material(IRenderDevice& RenderDevice, const std::weak_ptr<CMapTile> _parentADT) :
	MaterialProxie(RenderDevice.GetObjectsFactory().CreateMaterial(sizeof(MaterialProperties)))
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();
}

ADT_MCNK_Material::~ADT_MCNK_Material()
{
	if (m_pProperties)
	{
		_aligned_free(m_pProperties);
		m_pProperties = nullptr;
	}
}

//--

void ADT_MCNK_Material::SetLayersCnt(uint32 value)
{
	m_pProperties->m_LayersCnt = value;
	MarkConstantBufferDirty();
}

void ADT_MCNK_Material::SetShadowMapExists(uint32 value)
{
	m_pProperties->m_ShadowMapExists = value;
	MarkConstantBufferDirty();
}

//--

void ADT_MCNK_Material::UpdateConstantBuffer() const
{
	MaterialProxie::UpdateConstantBuffer(m_pProperties, sizeof(MaterialProperties));
}
