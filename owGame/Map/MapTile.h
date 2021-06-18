#pragma once

#include "MapChunk.h"
#include "Map/Instances/MapM2Instance.h"
#include "Map/Instances/MapWMOInstance.h"

// FORWARD BEGIN
class CMap;
// FORWARD END

class ZN_API CMapTile
	: public CSceneNode
	, public CLoadableObject
{
public:
	CMapTile(IScene& Scene, const CMap& MapParent, uint32 IndexX, uint32 IndexZ);
	virtual ~CMapTile();

	const CMap&                                     GetMap() const;
	int                                             getIndexX() const;
	int                                             getIndexZ() const;
	const CMapChunk*                                getChunk(int32 x, int32 z) const;
	bool                                            IsNortrend() const;
	std::shared_ptr<ADT_TextureInfo>                GetTextureInfo(size_t Index) const;

	// CSceneNode
	void											Initialize() override;

	// ILoadableObject
	bool                                            Load() override;
	bool                                            Delete() override;

private:
	const CMap&                                     m_MapParent;
	const int                                       m_IndexX;
	const int                                       m_IndexZ;

	ADT_MHDR                                        m_Header;
	std::vector<std::shared_ptr<ADT_TextureInfo>>	m_Textures;

	// Instances
#ifdef USE_WMO_MODELS
	std::vector<CMapWMOInstance*>	m_WMOsInstances;
#endif
#ifdef USE_M2_MODELS
	std::vector<CMapM2Instance*>	m_MDXsInstances;
#endif
	std::vector<CMapChunk*>			m_Chunks;
};
