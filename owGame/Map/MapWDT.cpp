#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "MapWDT.h"

CMapWDT::CMapWDT(const CMap& Map)
	: m_Map(Map)
	
	, m_IsTileBased(false)
	
#ifdef USE_WMO_MODELS
	, m_GlobalWMO(nullptr)
#endif
{}

CMapWDT::~CMapWDT()
{
}

void CMapWDT::CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const
{
	Log::Info("MapWDT: Global WMO exists [%s].", false == m_GlobalWMOName.empty() ? "true" : "false");

	if (false == m_GlobalWMOName.empty())
	{
#ifdef USE_WMO_MODELS
		if (std::shared_ptr<CWMO> wmo = m_Map.GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadWMO(m_Map.GetBaseManager().GetApplication().GetRenderDevice(), m_GlobalWMOName))
		{
			auto inst = Parent->CreateSceneNode<CMapWMOInstance>(wmo, m_GlobalWMOPlacementInfo);
			m_Map.GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(inst);

			std::const_pointer_cast<CMapWMOInstance>(m_GlobalWMO) = inst;
		}
#endif
	}
}

void CMapWDT::Load()
{
    WoWChunkReader reader(m_Map.GetBaseManager(), m_Map.GetMapFolder() + ".wdt");

	if (auto buffer = reader.OpenChunk("MVER"))
    {
        uint32 version;
        buffer->readBytes(&version, 4);
        _ASSERT(version == 18);
    }

	if (auto buffer = reader.OpenChunk("MPHD"))
    {
        buffer->readBytes(&m_MPHD, sizeof(WDT_MPHD));
    }

	if (auto buffer = reader.OpenChunk("MAIN"))
    {
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

	if (auto buffer = reader.OpenChunk("MWMO"))
    {
        char* buf = new char[buffer->getSize()];
        buffer->readBytes(buf, buffer->getSize());
        m_GlobalWMOName = std::string(buf);
        delete[] buf;
    }

#ifdef USE_WMO_MODELS
	if (auto buffer = reader.OpenChunk("MODF"))
    {
        _ASSERT(m_MPHD.flags.Flag_GlobalWMO);
        _ASSERT((buffer->getSize() / sizeof(SMapTile_MODF)) == 1);
        buffer->readBytes(&m_GlobalWMOPlacementInfo, sizeof(SMapTile_MODF));
    }
#endif

	_ASSERT(m_IsTileBased || m_GlobalWMOName.size() > 0);
}
