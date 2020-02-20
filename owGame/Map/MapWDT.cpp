#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "MapWDT.h"

// Additional
#include "WoWChunkReader.h"

CMapWDT::CMapWDT(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CMap& Map)
	: m_IsTileBased(false)
	, m_GlobalWMO(nullptr)
	
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_Map(Map)
{}

CMapWDT::~CMapWDT()
{
}

void CMapWDT::CreateInsances(const std::shared_ptr<ISceneNode3D>& Parent) const
{
	Log::Green("Map_GlobalWMOs[]: Global WMO exists [%s].", !m_GlobalWMOName.empty() ? "true" : "false");
	if (!m_GlobalWMOName.empty())
	{
		_ASSERT(false);
		//m_GlobalWMO = _parent->CreateSceneNode<CMapWMOInstance>(m_GlobalWMOName);
		//m_GlobalWMO->Initialize(m_GlobalWMOPlacementInfo);
	}
}

void CMapWDT::Load()
{
    std::string fileName = m_Map.GetMapFolder() + ".wdt";

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
