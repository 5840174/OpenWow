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
	CMapChunk(IScene& Scene, CMapTile& MapTileParent, const SMapTile_MCIN& Chunk, const std::shared_ptr<IByteBuffer>& Bytes);
	virtual ~CMapChunk();

	// CMapChunk
	uint32 GetAreaID() const;
	float GetTerrainHeight(glm::vec3 Position) const;
	void ExtendMapChunkBounds(const BoundingBox& OtherBBox);

	// ISceneNode
	void Initialize() override;

	// ILoadable
	bool Load() override;
	bool Delete() override;

private:
	float m_Heights[C_MapBufferSize];

private:
	CMapTile&                       m_MapTile;
	CMap&                           m_Map;
	SMapChunk_MCNK                  m_Header;
	std::shared_ptr<IByteBuffer>    m_Bytes;
};
