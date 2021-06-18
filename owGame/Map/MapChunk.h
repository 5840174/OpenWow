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
	CMapChunk(IScene& Scene, const CMapTile& MapTileParent, const ADT_MCIN& Chunk, const std::shared_ptr<IByteBuffer>& Bytes);
	virtual ~CMapChunk();

	// CMapChunk
	uint32 GetAreaID() const;

	// SceneNode3D
	void Initialize() override;

	// ILoadable
	bool Load() override;
	bool Delete() override;

private:
	const CMapTile&                 m_MapTile;
	const CMap&                     m_Map;
	ADT_MCNK_Header                 m_Header;
	std::shared_ptr<IByteBuffer>    m_Bytes;
};
