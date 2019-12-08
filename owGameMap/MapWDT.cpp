#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "MapWDT.h"

CMapWDT::CMapWDT(IBaseManager* BaseManager, std::weak_ptr<const CMap> _mapController) 
	: m_IsTileBased(false)
	, m_GlobalWMO(nullptr)
	, m_MapController(_mapController)
	, m_BaseManager(BaseManager)
{}

CMapWDT::~CMapWDT()
{
}

void CMapWDT::CreateInsances(std::weak_ptr<ISceneNode> _parent)
{
	Log::Green("Map_GlobalWMOs[]: Global WMO exists [%s].", !m_GlobalWMOName.empty() ? "true" : "false");
	if (!m_GlobalWMOName.empty())
	{
		m_GlobalWMO = _parent.lock()->CreateWrappedSceneNode<CMapWMOInstance>("SceneNode3D", m_GlobalWMOName);
		m_GlobalWMO->Initialize(m_GlobalWMOPlacementInfo);

		GetManager<ILoader>(m_BaseManager)->AddToLoadQueue(m_GlobalWMO);
	}
}

void CMapWDT::Load()
{
    std::shared_ptr<const CMap> mapController = m_MapController.lock();
    _ASSERT(mapController != NULL);

    std::string fileName = mapController->GetMapFolder() + ".wdt";

    WoWChunkReader reader(m_BaseManager, fileName);
    std::shared_ptr<IByteBuffer> buffer = nullptr;

    {
        buffer = reader.OpenChunk("MVER");
        uint32 version;
        buffer->readBytes(&version, 4);
        _ASSERT(version == 18);
    }


    {
        buffer = reader.OpenChunk("MPHD");
        buffer->readBytes(&m_MPHD, sizeof(WDT_MPHD));
    }


    {
        buffer = reader.OpenChunk("MAIN");

        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                buffer->readBytes(&m_TileFlag[j][i], sizeof(WDT_MAIN));

                if (m_TileFlag[j][i].flags.Flag_HasADT)
                    m_IsTileBased = true;
            }
        }
    }


    {
        buffer = reader.OpenChunk("MWMO");

        if (buffer && buffer->getSize() > 0)
        {
            char* buf = new char[buffer->getSize()];
            buffer->readBytes(buf, buffer->getSize());
            m_GlobalWMOName = std::string(buf);
            delete[] buf;
        }
    }


    {
        buffer = reader.OpenChunk("MODF");

        if (buffer != nullptr)
        {
            _ASSERT(m_MPHD.flags.Flag_GlobalWMO);
            _ASSERT((buffer->getSize() / sizeof(ADT_MODF)) == 1);
            buffer->readBytes(&m_GlobalWMOPlacementInfo, sizeof(ADT_MODF));
        }
    }


	_ASSERT(m_IsTileBased || m_GlobalWMOName.size() > 0);
}
