#pragma once

#include "MapTile.h"
#include "MapWDT.h"
#include "MapWDL.h"
#include "MinimapProvider.h"

// FORWARD BEGIN
class CWorldClient;
// FORWARD END

class ZN_API CMap
	: public CSceneNode
{
public:
	CMap(IScene& Scene, CWorldClient& WorldClient);
	virtual ~CMap();

	void                                            MapPreLoad(const DBC_MapRecord* DBCMapRecord);
	void                                            MapLoad();
	void                                            Unload();

	void                                            EnterMap(glm::vec3 CameraPosition);
	void                                            EnterMap(int32 x, int32 z);
	std::shared_ptr<CMapTile>                       LoadTile(int32 x, int32 z);

	void                                            ClearCache();

	const CMapTile*                                 GetMapTileByPosition(glm::vec3 Position);
	const CMapChunk*                                GetMapChunkByPosition(glm::vec3 Position);
	uint32                                          GetAreaIDByPosition(glm::vec3 Position);
	const DBC_AreaTableRecord*                      GetAreaByPosition(glm::vec3 Position);
	float                                           GetTerrainHeight(glm::vec3 Position);

	// ISceneNode
	void                                            Update(const UpdateEventArgs& e) override;

public: // Getters
	uint32                                          GetMapID() const;
	std::string                                     GetMapFolder() const;

	bool                                            IsNortrend() const;
	bool                                            isUncompressedAlpha() const { return m_WDT->IsMapTileUse8BitAlphaMCAL(); }
#ifdef USE_WMO_MODELS
	std::shared_ptr<CMapWMOInstance>                getGlobalInstance() const { return m_WDT->GetGlobalWMOInstance(); }
#endif
	int                                             GetCurrentX() const { return m_CurrentTileX; }
	int                                             GetCurrentZ() const { return m_CurrentTileZ; }

	bool                                            IsTileCurrent(int32 x, int32 z) const;
	bool                                            IsTileInCurrent(const CMapTile& _mapTile);

	std::shared_ptr<ITexture>                       getMinimap() const { return m_WDL->getMinimap(); }

public: // shared
	std::vector<uint16>                             GenarateHighMapArray(uint16 Holes = 0) const;
	std::vector<uint16>                             GenarateDefaultMapArray(uint16 Holes = 0) const;
	std::shared_ptr<IBuffer>                        GetBufferTextureCoordDetailAndAlpha() const;

private:
	CWorldClient&                                   m_WorldClient;

	const DBC_MapRecord*                            m_MapDBCRecord;

	std::shared_ptr<CMapTile>	                    m_MapTilesCache[C_TilesCacheSize];
	std::shared_ptr<CMapTile>	                    m_MapTilesCurrent[C_RenderedTiles][C_RenderedTiles];

	int32					                        m_CurrentTileX;
	int32					                        m_CurrentTileZ;
	bool					                        m_IsOnInvalidTile;

	std::unique_ptr<CMapWDT>	                    m_WDT;
	std::unique_ptr<CMapWDL>	                    m_WDL;


private: // shared
	std::vector<uint16>                             m_HighMapStrip;
	std::vector<uint16>                             m_DefaultMapStrip;
	std::shared_ptr<IBuffer>                        m_BufferTextureCoordDetailAndAlpha;
	uint16                                          m_OuterArray[9][9];
	uint16                                          m_InnerArray[8][8];

	// Minimap
	CMinimapProvider*		                        mProvider;
	MinimapDir*				                        dir;
};
