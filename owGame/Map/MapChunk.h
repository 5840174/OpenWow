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
	CMapChunk(IRenderDevice& RenderDevice, const CMap& Map, const CMapTile& MapTile, const std::string& FileName, const ADT_MCIN& MCIN);
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
	std::shared_ptr<IFile>                          m_File;
	ADT_MCNK_Header                                 header;

	ADT_MCNK_MCLY                                   mcly[4];
	std::shared_ptr<ITexture>                        m_DiffuseTextures[4];
	std::shared_ptr<ITexture>                        m_SpecularTextures[4];

	std::shared_ptr<ITexture>                        m_BlendRBGShadowATexture;

private:
	IRenderDevice&                  m_RenderDevice;
	const CMap&						m_MapController;
	const CMapTile&			        m_ParentADT;

	const std::string               m_FileName;
	const ADT_MCIN                  m_MCIN;
};
