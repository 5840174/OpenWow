#pragma once

#include "MapChunk.h"
#include "Instances/MapM2Instance.h"
#include "Instances/MapWMOInstance.h"

// FORWARD BEGIN
class CWorldClient;
class CMap;
// FORWARD END

class ZN_API CMapTile
	: public CSceneNode
	, public CLoadableObject
{
public:
	CMapTile(IScene& Scene, CWorldClient& WorldClient, CMap& MapParent, uint32 IndexX, uint32 IndexZ);
	virtual ~CMapTile();
	
	// CMapTile
	CMap&                                           GetMap() const;
	int                                             GetTileIndexX() const;
	int                                             GetTileIndexZ() const;
	const CMapChunk*                                GetMapChunk(int32 x, int32 z) const;
	std::shared_ptr<SMapTile_MTEX>                  GetTextureInfo(size_t Index) const;
	void                                            ExtendMapTileBounds(const BoundingBox& OtherBBox);

	// ISceneNode
	void											Initialize() override;

	// ILoadableObject
	bool                                            Load() override;
	bool                                            Delete() override;

private:
	CWorldClient&                                   m_WorldClient;
	CMap&                                           m_MapParent;
	const int                                       m_IndexX;
	const int                                       m_IndexZ;

	SMapTile_MHDR                                   m_Header;
	std::vector<std::shared_ptr<SMapTile_MTEX>>	    m_Textures;

	// Instances
#ifdef USE_WMO_MODELS
	std::vector<CMapWMOInstance*>	                m_WMOsInstances;
#endif
#ifdef USE_M2_MODELS
	std::vector<CMapM2Instance*>	                m_MDXsInstances;
#endif
	std::vector<CMapChunk*>			                m_Chunks;
};
