#pragma once

// OpenWoW
#include "WoWChunkReader.h"

// WMO
#include "WMO_Headers.h"

// WMO Parts
#include "WMO_Group.h"
#include "WMO_Part_Fog.h"
#include "WMO_Part_Light.h"
#include "WMO_Part_Material.h"
#include "WMO_Part_Portal.h"

#include "WMO_PortalsController.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
// FORWARD END

class ZN_API CWMO 
	: public ISceneNodeProvider
	, public CLoadableObject
	, public std::enable_shared_from_this<CWMO>
{
	friend CWMO_Part_Portal;
public:
	CWMO(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::string& FileName);
	virtual ~CWMO();

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<ISceneNode3D>& Parent) const override;

	// CLoadableObject
	bool Load() override;

public:
	std::string getFilename() const { return m_FileName; }
	BoundingBox	GetBounds() const { return m_Bounds; }

	bool useAmbColor() const { return !(m_Header.flags.skip_base_color); }
	std::shared_ptr<M2> getSkybox() { return m_Skybox; }

public:
	
	SWMO_HeaderDef                                                      m_Header;				// MOHD chunk
	BoundingBox                                                         m_Bounds;

public:
	//-- Materials --//
	std::unique_ptr<char[]>												m_TexturesNames;		// MOTX chunk
	std::vector<std::shared_ptr<WMO_Part_Material>>						m_Materials;			// MOMT chunk

	//-- Groups --//
	std::unique_ptr<char[]>												m_GroupNames;		    // MOGN chunk
	std::vector<std::shared_ptr<WMO_Group>>                             m_Groups;				// MOGI chunk
	std::vector<std::shared_ptr<WMO_Group>>                             m_OutdoorGroups;

	//-- Skybox --//
	std::shared_ptr<M2>                                                 m_Skybox;

	//-- Portals --//
#ifndef WMO_DISABLE_PORTALS
	std::shared_ptr<CWMO_PortalsController>                             m_PortalController;
#endif
	
	//-- Visible block
	std::vector<vec3>                                                   m_VisibleBlockVertices;	// MOVV chunk
	std::vector<SWMO_VisibleBlockListDef>                               m_VisibleBlockList;		// MOVB chunk


	// -- Lights --//
	std::vector<std::shared_ptr<WMO_Part_Light>>                        m_Lights;				// MOLT chunk


	//-- Doodads --//
	std::vector<SWMO_Doodad_SetInfo>                                    m_DoodadsSetInfos;      // MODS chunk
	char*                                                               m_DoodadsFilenames;     // MODN chunk        
	std::vector<SWMO_Doodad_PlacementInfo>                              m_DoodadsPlacementInfos;


	//-- Fog --//
	std::vector<std::shared_ptr<WMO_Part_Fog>>                          m_Fogs;					// MFOG chunk


	//-- Volumes plane --//
	// MCVP chunk (optional)	

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	const std::string m_FileName;
	std::unique_ptr<WoWChunkReader> m_ChunkReader;
};
