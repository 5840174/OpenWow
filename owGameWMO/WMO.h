﻿#pragma once

#include "WMO_Headers.h"

// Parts
#include "WMO_Group.h"
#include "WMO_Part_Fog.h"
#include "WMO_Part_Light.h"
#include "WMO_Part_Material.h"
#include "WMO_Part_Portal.h"

#include "WMO_PortalsController.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
// FORWARD END

class WMO : public std::enable_shared_from_this<WMO>
{
	friend CWMO_Part_Portal;
public:
	WMO(cstring _fileName);
	~WMO();

	void CreateInsances(std::weak_ptr<CWMO_Base_Instance> _parent);
	bool Load();

	void Render(CWMO_Base_Instance* _localContr) const;

	bool drawSkybox();

#pragma region Getters
public:
	std::string getFilename() const { return m_FileName; }
	BoundingBox	getBounds() const { return m_Bounds; }

	bool useAmbColor() const { return !(m_Header.flags.skip_base_color); }
	std::shared_ptr<M2> getSkybox() { return m_Skybox; }
#pragma endregion

public:
	const std::string						m_FileName;
	SWMO_HeaderDef							m_Header;				// MOHD chunk
	BoundingBox								m_Bounds;

public:
	//-- Materials --//
	char*									m_TexturesNames;		// MOTX chunk
	std::vector<std::shared_ptr<const WMO_Part_Material>>				m_Materials;			// MOMT chunk

	//-- Groups --//

	std::vector<std::shared_ptr<WMO_Group>>						m_Groups;				// MOGI chunk
	std::vector<std::shared_ptr<WMO_Group>>						m_OutdoorGroups;

	//-- Skybox --//
	std::shared_ptr<M2>								m_Skybox;

	//-- Portals --//
	std::vector<vec3>						m_PortalVertices;		// MOPV chunk
	std::shared_ptr<Buffer>							m_PortalVB;
	std::vector<std::shared_ptr<CWMO_Part_Portal>>			m_Portals;
	std::vector<SWMO_PortalReferencesDef>	m_PortalReferences;		// MOPR chunk
	std::shared_ptr<CWMO_PortalsController>					m_PortalController;

	//-- Visible block
	std::vector<vec3>						m_VisibleBlockVertices;	// MOVV chunk
	std::vector<SWMO_VisibleBlockListDef>	m_VisibleBlockList;		// MOVB chunk


	// -- Lights --//
	std::vector<std::shared_ptr<WMO_Part_Light>>			m_Lights;				// MOLT chunk


	//-- Doodads --//
	std::vector<SWMO_Doodad_SetInfo>		m_DoodadsSetInfos;			// MODS chunk
	char*									m_DoodadsFilenames;			// MODN chunk        
	std::vector<SWMO_Doodad_PlacementInfo>  m_DoodadsPlacementInfos;


	//-- Fog --//
	std::vector<std::shared_ptr<WMO_Part_Fog>>				m_Fogs;					// MFOG chunk


	//-- Volumes plane --//
	// MCVP chunk (optional)	
};
