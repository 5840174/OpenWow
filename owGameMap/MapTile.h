#pragma once

#include "MapChunk.h"
#include "MapM2Instance.h"
#include "MapWMOInstance.h"

#include "Map_Headers.h"

// FORWARD BEGIN
class CMap;
// FORWARD END

class CMapTile 
	: public CSceneNodeProxie
	, public CLoadableObject
{
public:
	                                                CMapTile();
	virtual                                         ~CMapTile();

	void											Initialize() override;


    void                                            Initialize(uint32 _intexX, uint32 _intexZ);

    std::shared_ptr<CMapChunk>                      getChunk(int32 x, int32 z);

	// SceneNode3D
	bool                                            Accept(IVisitor* visitor) override;

	// ILoadableObject
	bool                                            Load() override;
	bool                                            Delete() override;
	uint32                                          getPriority() const override { return 0; };

public:
    mutable int										m_IndexX;
    mutable int										m_IndexZ;
	
	ADT_MHDR                                        header;
	std::vector<std::shared_ptr<ADT_TextureInfo>>	m_Textures;

	// Instances
	std::vector<std::shared_ptr<CMapWMOInstance>>	m_WMOsInstances;
	std::vector<std::shared_ptr<ISceneNode>>		m_MDXsInstances;
	std::vector<std::shared_ptr<CMapChunk>>			m_Chunks;

protected:
    std::shared_ptr<CMap>							GetMapController() const;
};
