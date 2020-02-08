#pragma once

#include "Map_Headers.h"

// FORWARD BEGIN
class CMapTile;
class CMap;
// FORWARD END

class CMapChunk 
	: public SceneNode3D
	, public CLoadableObject
{
public:
	                                                CMapChunk(CMap* _mapController, std::weak_ptr<CMapTile> _parentTile);
	virtual                                         ~CMapChunk();

	void                                            Initialize() override;

    void                                            Initialize(const std::string& _fileName, const ADT_MCIN& _mcin);

    uint32                                          GetAreaID() const;

	// SceneNode3D
	std::string                                     GetName() const override
	{
		return "MapChunk " + std::to_string(mcin.offset);
	}

	bool                                            Accept(IVisitor* visitor) override;

	// ILoadable
	bool                                            PreLoad() override;
	bool                                            Load() override;
	bool                                            Delete() override;
	uint32                                          getPriority() const override { return 1; };

private:
	std::string                                     m_FileName;
	std::shared_ptr<IFile>                          m_File;
	mutable ADT_MCIN                                mcin;
	ADT_MCNK_Header                                 header;

	ADT_MCNK_MCLY                                   mcly[4];
	std::shared_ptr<ITexture>                        m_DiffuseTextures[4];
	std::shared_ptr<ITexture>                        m_SpecularTextures[4];

	std::shared_ptr<ITexture>                        m_BlendRBGShadowATexture;

	// Qulity
	std::shared_ptr<IModel>                          __geomHigh;
	std::shared_ptr<IModel>                          __geomDefault;

private:
	CMap*						m_MapController;
	const std::weak_ptr<CMapTile>			        m_ParentADT;
	//const CGroupQuality*						    m_QualitySettings;
};
