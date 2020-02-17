#pragma once

#include "Map_Shared.h"
#include "Sky/SkyManager.h"
#include "Sky/Environment/EnvironmentManager.h"
#include "MapTile.h"
#include "MapWDT.h"
#include "MapWDL.h"
#include "MinimapProvider.h"

class ZN_API CMap
	: public SceneNode3D
{
public:
	CMap(IBaseManager* BaseManager, IRenderDevice& RenderDevice);
	virtual ~CMap();

	void                                            MapPreLoad(const DBC_MapRecord* _map);
	void                                            MapLoad();
	void                                            MapPostLoad();
	void                                            Unload();

	//
	void                                            EnterMap(vec3 _cameraPosition);
	void                                            EnterMap(int32 x, int32 z);
	CMapTile*                                       LoadTile(int32 x, int32 z);
	void                                            ClearCache();
	uint32                                          GetAreaID(const ICameraComponent3D* camera);

	// Scene node
	std::string										GetName() const override
	{
		return "Map";
	}

	void                                            Update(const UpdateEventArgs& e) override;

public: // Getters
	std::string                                     GetMapFolder() const { return m_MapFolderName; }

	bool                                            isUncompressedAlpha() const { return m_WDT->getFlags().Flag_8bitMCAL; }
	bool                                            isTileBased() const { return m_WDT->MapHasTiles(); }
	const CMapWMOInstance*							getGlobalInstance() const { return m_WDT->GetGlobalWMOInstance(); }
	int                                             GetCurrentX() const { return m_CurrentTileX; }
	int                                             GetCurrentZ() const { return m_CurrentTileZ; }

	void                                            SetOutOfBounds(bool _value) { m_IsOnInvalidTile = _value; }
	bool                                            IsOutOfBounds() const { return m_IsOnInvalidTile; }

	std::shared_ptr<ITexture>                       getMinimap() const { return m_WDL->getMinimap(); }

	bool                                            getTileIsCurrent(int x, int z) const;
	bool                                            IsTileInCurrent(const CMapTile& _mapTile);


private:
	std::string                                     m_MapFolderName;
	const DBC_MapRecord*                            m_MapDBCRecord;


	CMapTile*	                                    m_ADTCache[C_TilesCacheSize];
	CMapTile*	                                    m_Current[C_RenderedTiles][C_RenderedTiles];
	int32					                        m_CurrentTileX, m_CurrentTileZ;
	bool					                        m_IsOnInvalidTile;

	std::unique_ptr<CMapWDT>	                    m_WDT;
	std::unique_ptr<CMapWDL>	                    m_WDL;

	// Minimap
	CMinimapProvider*		                        mProvider;
	MinimapDir*				                        dir;

private:
	IBaseManager*									m_BaseManager;
	IRenderDevice&                                  m_RenderDevice;
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