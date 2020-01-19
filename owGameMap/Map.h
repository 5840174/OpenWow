#pragma once

#include "Map_Shared.h"
#include "SkyManager.h"
#include "EnvironmentManager.h"
#include "MapTile.h"
#include "MapWDT.h"
#include "MapWDL.h"
#include "MinimapProvider.h"

class CMap 
	: public SceneNodeBase
{
public:
	CMap(IBaseManager* BaseManager);
	virtual ~CMap();

	void                                            MapPreLoad(std::shared_ptr<DBC_MapRecord> _map);
	void                                            MapLoad();
	void                                            MapPostLoad();
	void                                            Unload();

	//
	void                                            EnterMap(vec3 _cameraPosition);
	void                                            EnterMap(int32 x, int32 z);
	std::shared_ptr<CMapTile>                       LoadTile(int32 x, int32 z);
	void                                            ClearCache();
	uint32                                          GetAreaID(ICameraComponent3D* camera);

	// Scene node
	std::string										GetName() const override
	{
		return "Map";
	}

	void                                            UpdateCamera(const ICameraComponent3D* camera) override;

public: // Getters
	std::string                                     GetMapFolder() const { return m_MapFolderName; }
    std::shared_ptr<DBC_MapRecord>                  GetMapDBCRecord() const { return m_MapDBCRecord; }

	bool                                            isUncompressedAlpha() const { return m_WDT->getFlags().Flag_8bitMCAL; }
	bool                                            isTileBased() const { return m_WDT->MapHasTiles(); }
	const std::shared_ptr<CMapWMOInstance>          getGlobalInstance() const { return m_WDT->GetGlobalWMOInstance(); }
	int                                             GetCurrentX() const { return m_CurrentTileX; }
	int                                             GetCurrentZ() const { return m_CurrentTileZ; }

	void                                            SetOutOfBounds(bool _value) { m_IsOnInvalidTile = _value; }
	bool                                            IsOutOfBounds() const { return m_IsOnInvalidTile; }
	
	std::shared_ptr<ITexture>                        getMinimap() const { return m_WDL->getMinimap(); }
	const std::shared_ptr<SkyManager>               getSkyManager() const { return m_SkyManager; }
	const std::shared_ptr<EnvironmentManager>       getEnvManager() const { return m_EnvironmentManager; }
	DayNightPhase                                   getDayNightPhase() const { return m_EnvironmentManager->dayNightCycle->getPhase(m_GameTime.GetTime()); }
	WowTime*                                        getTime() { return &m_GameTime; }

    bool                                            getTileIsCurrent(int x, int z) const;
    bool                                            IsTileInCurrent(std::shared_ptr<CMapTile> _mapTile);
	

private:
	std::string                                     m_MapFolderName;
    std::shared_ptr<DBC_MapRecord>                  m_MapDBCRecord;

	std::shared_ptr<CMapTile>	                    m_ADTCache[C_TilesCacheSize];
	std::shared_ptr<CMapTile>	                    m_Current[C_RenderedTiles][C_RenderedTiles];
	int32					                        m_CurrentTileX, m_CurrentTileZ;
	bool					                        m_IsOnInvalidTile;

	std::shared_ptr<CMapWDT>	                    m_WDT;
	std::shared_ptr<CMapWDL>	                    m_WDL;
	std::shared_ptr<SkyManager>                     m_SkyManager;
	std::shared_ptr<EnvironmentManager>             m_EnvironmentManager;
	WowTime					                        m_GameTime;

	// Minimap
	CMinimapProvider*		                        mProvider;
	MinimapDir*				                        dir;

private:
	IBaseManager*									m_BaseManager;
};

inline static bool IsBadTileIndex(int i, int j)
{
	if (i < 0)
		return true;

	if (j < 0)
		return true;

	if (i >= C_TilesInMap)
		return true;

	if (j >= C_TilesInMap)
		return true;

	return false;
}

inline static bool IsGoodTileIndex(int i, int j)
{
	return (!IsBadTileIndex(i, j));
}