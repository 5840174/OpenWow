#pragma once

#include "Map_Headers.h"

// FORWARD BEGIN
class CMap;
class CMapTile;
// FORWARD END

class ZN_API CMapChunk
	: public CSceneNode
	, public CLoadableObject
{
public:
	CMapChunk(IScene& Scene, CMapTile& MapTileParent, const ADT_MCIN& Chunk, const std::shared_ptr<IByteBuffer>& Bytes);
	virtual ~CMapChunk();

	// CMapChunk
	uint32 GetAreaID() const;
	void ExtendMapChunkBounds(const BoundingBox& OtherBBox);

	// SceneNode3D
	void Initialize() override;

	// ILoadable
	bool Load() override;
	bool Delete() override;

private:
	CMapTile&                       m_MapTile;
	CMap&                           m_Map;
	ADT_MCNK_Header                 m_Header;
	std::shared_ptr<IByteBuffer>    m_Bytes;
};
