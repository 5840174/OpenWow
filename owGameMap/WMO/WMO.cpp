#include "stdafx.h"

// Include
#include "WMO_Base_Instance.h"

// General
#include "WMO.h"

struct WMO_GroupInfoDef
{
	SWMOGroup_Flags	flags;
	CAaBox			bounding_box;
	int32			nameoffset;		// name in MOGN chunk (-1 for no name)
};

WMO::WMO(const std::string& name) :
	m_FileName(name),
	m_TexturesNames(nullptr),
	m_DoodadsFilenames(nullptr)
{}

WMO::~WMO()
{
	Log::Info("WMO[%s]: Unloading...", m_FileName.c_str());

	//

	SafeDeleteArray(m_TexturesNames);
	SafeDeleteArray(m_DoodadsFilenames);
}

//
// ISceneNodeProvider
//
void WMO::CreateInsances(std::weak_ptr<SceneNodeModel3D> _parent)
{
	std::shared_ptr<CWMO_Base_Instance> parentAsWMOInstance = std::dynamic_pointer_cast<CWMO_Base_Instance, SceneNode3D>(_parent.lock());
	assert1(parentAsWMOInstance != nullptr);

	for (auto& it : m_Groups)
	{
		std::shared_ptr<CWMO_Group_Instance> groupInstance = std::make_shared<CWMO_Group_Instance>(it);
		groupInstance->SetParent(_parent);

		parentAsWMOInstance->AddGroupInstance(groupInstance);
		if (it->m_Header.flags.IS_OUTDOOR)
		{
			parentAsWMOInstance->AddOutdoorGroupInstance(groupInstance);
		}

		it->CreateInsances(groupInstance);
	}

	for (auto& it : m_Lights)
	{
		parentAsWMOInstance->AddLight(it);
	}
}

bool WMO::Load()
{
	WoWChunkReader reader(m_FileName);

	std::shared_ptr<IByteBuffer> buffer = nullptr;

	// Version
	{
		buffer = reader.OpenChunk("MVER");
		uint32 version;
		buffer->readBytes(&version, 4);
		assert1(version == 17);
	}

	// Header
	{
		buffer = reader.OpenChunk("MOHD");
		buffer->readBytes(&m_Header, sizeof(SWMO_HeaderDef));
		m_Bounds.set(m_Header.bounding_box.min, m_Header.bounding_box.max, true);
	}

	// Textures
	{
		buffer = reader.OpenChunk("MOTX");
		m_TexturesNames = new char[buffer->getSize() + 1];
		buffer->readBytes(m_TexturesNames, buffer->getSize());
		m_TexturesNames[buffer->getSize()] = 0x00;
	}

	// Materials
	{
		for (auto mat : reader.OpenChunkT<SWMO_MaterialDef>("MOMT"))
		{
			m_Materials.push_back(std::make_shared<WMO_Part_Material>(shared_from_this(), mat));
		}
		assert1(m_Materials.size() == m_Header.nTextures);
	}

	// Group names
	char* groupsNames = nullptr;
	{
		buffer = reader.OpenChunk("MOGN");
		if (buffer)
		{
			groupsNames = new char[buffer->getSize() + 1];
			buffer->readBytes(groupsNames, buffer->getSize());
			groupsNames[buffer->getSize()] = 0x00;
		}
	}

	// Group info
	{
		uint32 cntr = 0;
		for (auto groupInfo : reader.OpenChunkT<WMO_GroupInfoDef>("MOGI"))
		{
			char temp[256];
			strcpy_s(temp, m_FileName.c_str());
			temp[m_FileName.length() - 4] = 0;

			char fname[256];
			sprintf_s(fname, "%s_%03d.wmo", temp, cntr);
			std::shared_ptr<IFile> groupFile = GetManager<IFilesManager>()->Open(fname); // It delete later

			std::string groupName = groupFile->Name();
			if (groupInfo.nameoffset > 0)
				groupName = std::string(groupsNames + groupInfo.nameoffset);

			std::shared_ptr<WMO_Group> group = std::make_shared<WMO_Group>(shared_from_this(), cntr, groupName, groupFile);
			m_Groups.push_back(group);

			cntr++;
		}
	}

	// Skybox
	char* skyboxFilename = nullptr;
	{
		buffer = reader.OpenChunk("MOSB");
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
		for (auto pv : reader.OpenChunkT<vec3>("MOPV"))
		{
			m_PortalVertices.push_back(Fix_XZmY(pv));
		}

		if (! m_PortalVertices.empty())
			m_PortalVB = _RenderDevice->CreateVertexBuffer(m_PortalVertices);
	}

	// Portal defs
	{
		for (auto pt : reader.OpenChunkT<SWMO_PortalDef>("MOPT"))
		{
			m_Portals.push_back(std::make_shared<CWMO_Part_Portal>(shared_from_this(), pt));
		}
	}

	// Portal references
	{
		for (auto pr : reader.OpenChunkT<SWMO_PortalReferencesDef>("MOPR"))
		{
			m_PortalReferences.push_back(pr);
		}
	}

	// Visible vertices
	{
		for (auto vv : reader.OpenChunkT<vec3>("MOVV"))
		{
			m_VisibleBlockVertices.push_back(Fix_XZmY(vv));
		}
	}

	// Visible blocks
	{
		for (auto vb : reader.OpenChunkT<SWMO_VisibleBlockListDef>("MOVB"))
		{
			m_VisibleBlockList.push_back(vb);
		}
	}

	// Lights
	{
		for (auto lt : reader.OpenChunkT<SWMO_LightDef>("MOLT"))
		{
			m_Lights.push_back(std::make_shared<WMO_Part_Light>(lt));
		}
	}

	// Doodads set
	{
		for (auto ds : reader.OpenChunkT<SWMO_Doodad_SetInfo>("MODS"))
		{
			m_DoodadsSetInfos.push_back(ds);
		}
	}

	// Doodads filenames
	{
		buffer = reader.OpenChunk("MODN");
		if (buffer)
		{
			m_DoodadsFilenames = new char[buffer->getSize() + 1];
			buffer->readBytes(m_DoodadsFilenames, buffer->getSize());
			m_DoodadsFilenames[buffer->getSize()] = 0x00;
		}
	}

	// Doodads placemnts
	{
		for (auto dd : reader.OpenChunkT<SWMO_Doodad_PlacementInfo>("MODD"))
		{
			m_DoodadsPlacementInfos.push_back(dd);
		}

		// HACK! INCORRECT SIZE
		m_Header.nDoodadDefs = m_DoodadsPlacementInfos.size();
	}

	// Fog
	{
		for (auto fog : reader.OpenChunkT<SWMO_FogDef>("MFOG"))
		{
			m_Fogs.push_back(std::make_shared<WMO_Part_Fog>(fog));
		}
	}


	if (groupsNames) delete[] groupsNames;
	if (skyboxFilename) delete[] skyboxFilename;

	// Create portal controller
	if (m_Portals.size() > 0)
	{
		m_PortalController = std::make_shared<CWMO_PortalsController>(shared_from_this());

		for (auto& it : m_PortalReferences)
		{
			assert1(it.portalIndex < m_Portals.size());
			assert1(it.groupIndex < m_Groups.size());
		}
	}

	// Init m_Groups
	for (auto it : m_Groups)
	{
		it->Load();

		//assert1(it->m_Header.flags.IS_OUTDOOR != it->m_Header.flags.IS_INDOOR);

		// Add outdoor group
		if (it->m_Header.flags.IS_OUTDOOR)
		{
			m_OutdoorGroups.push_back(it);
		}
	}

	return true;
}

void WMO::Render(CWMO_Base_Instance* _localContr) const
{
	/*for (auto& it : m_Portals)
	{
		it->Render(_localContr->GetWorldTransfom());
	}*/


	for (auto& it : m_Lights)
	{
		//it->Render(_localContr->GetWorldTransfom());
	}
}

bool WMO::drawSkybox()
{
	/*if (m_Skybox == nullptr)
	{
		return false;
	}

	mat4 worldMatrix;
	worldMatrix = glm::translate(worldMatrix, _Render->getCamera()->Position);
	worldMatrix = glm::scale(worldMatrix, vec3(2.0f));*/

	//m_Skybox->Render(worldMatrix, nullptr, vec4(1.0f), 0, 0, 0);

	//_World->EnvM()->m_HasSky = true;

	return true;
}