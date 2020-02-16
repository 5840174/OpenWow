#include "stdafx.h"

// Include
#include "WMO_Base_Instance.h"

// General
#include "WMO.h"

CWMO::CWMO(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const std::string& Filename)
	: m_FileName(Filename)
	, m_TexturesNames(nullptr)
	, m_DoodadsFilenames(nullptr)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
{
	m_ChunkReader = std::make_unique<WoWChunkReader>(m_BaseManager, m_FileName);

	// Version
	{
		auto buffer = m_ChunkReader->OpenChunk("MVER");
		uint32 version;
		buffer->readBytes(&version, 4);
		_ASSERT(version == 17);
	}

	// Header
	{
		auto buffer = m_ChunkReader->OpenChunk("MOHD");
		buffer->readBytes(&m_Header, sizeof(SWMO_HeaderDef));

		m_Bounds = m_Header.bounding_box.Convert();
	}
}

CWMO::~CWMO()
{
	SafeDeleteArray(m_DoodadsFilenames);
}

//
// ISceneNodeProvider
//
void CWMO::CreateInsances(ISceneNode3D* _parent) const
{
	CWMO_Base_Instance* parentAsWMOInstance = dynamic_cast<CWMO_Base_Instance*>(_parent);
	_ASSERT(parentAsWMOInstance != nullptr);

	for (const auto& it : m_Groups)
	{
		CWMO_Group_Instance* groupInstance = _parent->CreateSceneNode<CWMO_Group_Instance>(*it);
		parentAsWMOInstance->AddGroupInstance(groupInstance);
		if (it->m_GroupHeader.flags.IS_OUTDOOR)
			parentAsWMOInstance->AddOutdoorGroupInstance(groupInstance);

		m_BaseManager->GetManager<ILoader>()->AddToLoadQueue(groupInstance);
	}

	for (auto& it : m_Lights)
	{
		//parentAsWMOInstance->AddLight(it);
	}
}

bool CWMO::Load()
{
	std::shared_ptr<IByteBuffer> buffer = nullptr;

	// Textures
	{
		buffer = m_ChunkReader->OpenChunk("MOTX");
		m_TexturesNames = std::unique_ptr<char[]>(new char[buffer->getSize() + 1]);
		buffer->readBytes(m_TexturesNames.get(), buffer->getSize());
		m_TexturesNames[buffer->getSize()] = 0x00;
	}

	// Materials
	size_t cntr = 0;
	{
		for (const auto& mat : m_ChunkReader->OpenChunkT<SWMO_MaterialDef>("MOMT"))
		{
			std::shared_ptr<WMO_Part_Material> material = std::make_shared<WMO_Part_Material>(m_RenderDevice, *this, mat);
			material->SetName(m_FileName + "_Material_" + std::to_string(cntr++));
			m_Materials.push_back(material);
		}
		_ASSERT(m_Materials.size() == m_Header.nTextures);
		Log::Info("WMO: Materials count = '%d'", m_Materials.size());
	}

	// Group names
	{
		buffer = m_ChunkReader->OpenChunk("MOGN");
		m_GroupNames = std::unique_ptr<char[]>(new char[buffer->getSize() + 1]);
		buffer->readBytes(m_GroupNames.get(), buffer->getSize());
		m_GroupNames[buffer->getSize()] = 0x00;
	}

	// Group info
	{
		uint32 cntr = 0;
		for (const auto& groupInfo : m_ChunkReader->OpenChunkT<SWMO_GroupInfoDef>("MOGI"))
		{
			std::shared_ptr<WMO_Group> group = std::make_shared<WMO_Group>(m_BaseManager, m_RenderDevice, *this, cntr++, groupInfo);
			m_BaseManager->GetManager<ILoader>()->AddToLoadQueue(group.get());
			m_Groups.push_back(group);

			if (group->m_GroupHeader.flags.IS_OUTDOOR)
			{
				m_OutdoorGroups.push_back(group);
			}
		}
	}

	// Skybox
	char* skyboxFilename = nullptr;
	{
		buffer = m_ChunkReader->OpenChunk("MOSB");
		if (buffer->getSize() > 4)
		{
			skyboxFilename = new char[buffer->getSize() + 1];
			buffer->readBytes(skyboxFilename, buffer->getSize());
			skyboxFilename[buffer->getSize()] = 0x00;
			Log::Error("WMO[%s]: Skybox [%s]", m_FileName.c_str(), skyboxFilename);
		}
	}

	// Portal vertices
	{
		for (const auto& pv : m_ChunkReader->OpenChunkT<glm::vec3>("MOPV"))
		{
			m_PortalVertices.push_back(Fix_XZmY(pv));
		}

		if (!m_PortalVertices.empty())
			m_PortalVB = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(m_PortalVertices);
	}

	// Portal defs
	{
		for (const auto& pt : m_ChunkReader->OpenChunkT<SWMO_PortalDef>("MOPT"))
		{
			m_Portals.push_back(std::make_shared<CWMO_Part_Portal>(m_RenderDevice, *this, pt));
		}
	}

	// Portal references
	{
		for (const auto& pr : m_ChunkReader->OpenChunkT<SWMO_PortalReferencesDef>("MOPR"))
		{
			m_PortalReferences.push_back(pr);
		}
	}

	// Visible vertices
	{
		for (const auto& vv : m_ChunkReader->OpenChunkT<vec3>("MOVV"))
		{
			m_VisibleBlockVertices.push_back(Fix_XZmY(vv));
		}
	}

	// Visible blocks
	{
		for (const auto& vb : m_ChunkReader->OpenChunkT<SWMO_VisibleBlockListDef>("MOVB"))
		{
			m_VisibleBlockList.push_back(vb);
		}
	}

	// Lights
	{
		for (const auto& lt : m_ChunkReader->OpenChunkT<SWMO_LightDef>("MOLT"))
		{
			m_Lights.push_back(std::make_shared<WMO_Part_Light>(lt));
		}
		Log::Info("WMO: Lights count = '%d'", m_Lights.size());
	}

	// Doodads set
	{
		for (const auto& ds : m_ChunkReader->OpenChunkT<SWMO_Doodad_SetInfo>("MODS"))
		{
			m_DoodadsSetInfos.push_back(ds);
		}

		Log::Info("WMO: Doodads count = '%d'", m_DoodadsSetInfos.size());
	}

	// Doodads filenames
	{
		buffer = m_ChunkReader->OpenChunk("MODN");
		if (buffer)
		{
			m_DoodadsFilenames = new char[buffer->getSize() + 1];
			buffer->readBytes(m_DoodadsFilenames, buffer->getSize());
			m_DoodadsFilenames[buffer->getSize()] = 0x00;
		}
	}

	// Doodads placemnts
	{
		for (const auto& dd : m_ChunkReader->OpenChunkT<SWMO_Doodad_PlacementInfo>("MODD"))
		{
			m_DoodadsPlacementInfos.push_back(dd);
		}

		// HACK! INCORRECT SIZE
		m_Header.nDoodadDefs = m_DoodadsPlacementInfos.size();
	}

	// Fog
	{
		for (const auto& fog : m_ChunkReader->OpenChunkT<SWMO_FogDef>("MFOG"))
		{
			m_Fogs.push_back(std::make_shared<WMO_Part_Fog>(fog));
		}
	}

	if (skyboxFilename) delete[] skyboxFilename;

	// Create portal controller
	if (m_Portals.size() > 0)
	{
#ifndef WMO_DISABLE_PORTALS
		m_PortalController = std::make_shared<CWMO_PortalsController>(*this);

		for (auto& it : m_PortalReferences)
		{
			_ASSERT(it.portalIndex < m_Portals.size());
			_ASSERT(it.groupIndex < m_Groups.size());
}
#endif
	}

	m_ChunkReader.reset();

	return true;
}
