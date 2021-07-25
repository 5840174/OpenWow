#pragma once

#include "WorldObjectsCreator.h"
#include "../Sky/SkyManager.h"
#include "../Map/Map.h"

class CWorldClient
{
public:
	CWorldClient(const IBaseManager& BaseManager, IScene& Scene);
	virtual ~CWorldClient();

	void Initialize();
	void Update(const UpdateEventArgs& e);
	void EnterWorld(uint32 MapID, int32 TileX, int32 TileZ);
	void EnterWorld(uint32 MapID, glm::vec3 Position);

	const std::shared_ptr<CWorldObjectCreator>& GetCreator() const { return m_Creator; }
	const std::shared_ptr<CSkyManager>& GetSky() const { return m_Sky; }
	const std::shared_ptr<CMap>& GetMap() const { return m_Map; }
	
	const IBaseManager& GetBaseManager() const { return m_BaseManager; }
	IScene& GetScene() const { return m_Scene; }

private:
	std::shared_ptr<CWorldObjectCreator> m_Creator;
	std::shared_ptr<CSkyManager> m_Sky;
	std::shared_ptr<CMap> m_Map;

private:
	const IBaseManager& m_BaseManager;
	IScene& m_Scene;
};