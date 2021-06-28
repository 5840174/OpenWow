#pragma once

#include "MapChunk.h"
#include "Instances/MapM2Instance.h"
#include "Instances/MapWMOInstance.h"

// FORWARD BEGIN
class CMap;
// FORWARD END

class ZN_API CMapTile
	: public CSceneNode
	, public CLoadableObject
{
public:
	CMapTile(IScene& Scene, CMap& MapParent, uint32 IndexX, uint32 IndexZ);
	virtual ~CMapTile();

	CMap&                                           GetMap() const;
	int                                             getIndexX() const;
	int                                             getIndexZ() const;
	const CMapChunk*                                getChunk(int32 x, int32 z) const;
	bool                                            IsNortrend() const;
	std::shared_ptr<SMapTile_MTEX>                GetTextureInfo(size_t Index) const;

	void                                            ExtendMapTileBounds(const BoundingBox& OtherBBox);

	// CSceneNode
	void											Initialize() override;

	// ILoadableObject
	bool                                            Load() override;
	bool                                            Delete() override;

private:
	CMap&                                           m_MapParent;
	const int                                       m_IndexX;
	const int                                       m_IndexZ;

	SMapTile_MHDR                                        m_Header;
	std::vector<std::shared_ptr<SMapTile_MTEX>>	m_Textures;
	std::shared_ptr<ITexture>                       m_ArrayTexture;

	// Instances
#ifdef USE_WMO_MODELS
	std::vector<CMapWMOInstance*>	                m_WMOsInstances;
#endif
#ifdef USE_M2_MODELS
	std::vector<CMapM2Instance*>	                m_MDXsInstances;
#endif
	std::vector<CMapChunk*>			                m_Chunks;
};
