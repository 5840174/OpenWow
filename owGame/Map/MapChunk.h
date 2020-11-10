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
	CMapChunk(IRenderDevice& RenderDevice, const CMap& Map, const std::shared_ptr<CMapTile>& MapTile, const ADT_MCIN& Chunk, const std::shared_ptr<IByteBuffer>& Bytes);
	virtual ~CMapChunk();

	uint32 GetAreaID() const;

	// SceneNode3D
	void Initialize() override;

	// ILoadable
	bool Load() override;
	bool Delete() override;

private:
	ADT_MCNK_Header                 header;

	uint32                          m_AreaID;

private:
	IRenderDevice&                  m_RenderDevice;
	const CMap&						m_Map;
	const CMapTile&			        m_MapTile;

	std::shared_ptr<IByteBuffer>    m_Bytes;
};
