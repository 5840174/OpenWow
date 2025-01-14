﻿#pragma once

#ifdef USE_WMO_MODELS

// WMO
#include "WMO_Headers.h"

// WMO Parts
#include "WMOGroup.h"
#include "WMO_Part_Fog.h"
#include "WMO_Part_Light.h"
#include "WMO_Part_Material.h"
#include "WMO_Part_Portal.h"

// M2
#include "../M2/M2.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
// FORWARD END

class ZN_API CWMO 
	: public CLoadableObject
	, public std::enable_shared_from_this<CWMO>
{
	friend CWMO_Part_Portal;

public:
	CWMO(const IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::string& FileName);
	virtual ~CWMO();

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<CWMO_Base_Instance>& Parent) const;

	// ILoadable
	bool Load() override;
	bool Delete() override;

public:
	std::string GetFilename() const { return m_FileName; }
	BoundingBox	GetBounds() const { return m_Bounds; }
	const SWMO_MOHD& GetHeader() const { return m_Header; }

	std::string GetTextureName(size_t Offset) const { return std::string(m_TexturesNames.get() + Offset); }
	const std::shared_ptr<WMO_Part_Material>& GetMaterial(size_t Index) const { return m_Materials.at(Index); }

	std::string GetGroupName(size_t Offset) const { return std::string(m_GroupNames.get() + Offset); }

	const std::vector<std::shared_ptr<WMO_Part_Light>>& GetLights() const { return m_Lights; }

	bool IsDoodadInSet(size_t SetIndex, uint16 DoodadIndex) const { return m_DoodadsSetInfos.at(SetIndex).InSet(DoodadIndex); }
	std::string GetDoodadFileName(size_t Offset) const { return std::string(m_DoodadsFilenames.get() + Offset); }
	const SWMO_MODD& GetDoodadPlacement(size_t Index) const { return m_DoodadsPlacementInfos.at(Index); }

	bool useAmbColor() const { return false == (m_Header.flags.skip_base_color); }

private:
	//-- Materials --//
	std::unique_ptr<char[]>												m_TexturesNames;		// MOTX chunk
	std::vector<std::shared_ptr<WMO_Part_Material>>						m_Materials;			// MOMT chunk


	//-- Groups --//
	std::unique_ptr<char[]>												m_GroupNames;		    // MOGN chunk
	std::vector<std::shared_ptr<CWMOGroup>>                             m_Groups;				// MOGI chunk


	//-- Skybox --//
	std::shared_ptr<CM2>                                                m_Skybox;

#if 0
	//-- Visible block
	std::vector<glm::vec3>                                              m_VisibleBlockVertices;	// MOVV chunk
	std::vector<SWMO_MOVB>                                              m_VisibleBlockList;		// MOVB chunk
#endif

	// -- Lights --//
	std::vector<std::shared_ptr<WMO_Part_Light>>                        m_Lights;				// MOLT chunk


	//-- Doodads --//
	std::vector<SWMO_MODS>                                              m_DoodadsSetInfos;      // MODS chunk
	std::unique_ptr<char[]>                                             m_DoodadsFilenames;     // MODN chunk        
	std::vector<SWMO_MODD>                                              m_DoodadsPlacementInfos;


	//-- Fog --//
	std::vector<std::shared_ptr<WMO_Part_Fog>>                          m_Fogs;					// MFOG chunk


	//-- Volumes plane --//
	// MCVP chunk (optional)	

private:
	const IBaseManager&                                                 m_BaseManager;
	IRenderDevice&                                                      m_RenderDevice;
	const std::string                                                   m_FileName;
	std::unique_ptr<WoWChunkReader>                                     m_ChunkReader;
	SWMO_MOHD                                                           m_Header;
	BoundingBox                                                         m_Bounds;
};

#endif
