#pragma once

#include "Map_Headers.h"

// FORWARD BEGIN
class CMap;
class CMapTile;
// FORWARD END

class ZN_API CMapChunk
	: public SceneNode3D
	, public CLoadableObject
{
public:
	CMapChunk(IRenderDevice& RenderDevice, const CMap& Map, const CMapTile& MapTile, const std::shared_ptr<IByteBuffer>& Bytes);
	virtual ~CMapChunk();


	uint32                                          GetAreaID() const;

	// SceneNode3D
	void                                            Initialize() override;
	std::string                                     GetName() const override;

	void                                            Accept(IVisitor* visitor) override;

	// ILoadable
	bool                                            PreLoad() override;
	bool                                            Load() override;
	bool                                            Delete() override;
	uint32                                          getPriority() const override { return 1; };

private:
	ADT_MCNK_Header                                 header;

	uint32                          m_AreaID;

private:
	IRenderDevice&                  m_RenderDevice;
	const CMap&						m_Map;
	const CMapTile&			        m_MapTile;

	std::shared_ptr<IByteBuffer>    m_File;
};
