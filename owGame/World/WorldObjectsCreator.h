#pragma once

// Factory
#include "../M2/M2.h"
#include "../WMO/WMO.h"

#include "../Creature/Creature.h"

#include "../Character/Character.h"

#include "../GameObject/GameObjectM2.h"
#include "../GameObject/GameObjectWMO.h"

// FORWARD BEGIN
class CWorldClient;
// FORWARD END

class ZN_API CWorldObjectCreator
{
public:
	CWorldObjectCreator(CWorldClient& WorldClient, const IBaseManager& BaseManager);

#ifdef USE_M2_MODELS
	// Factory
	std::shared_ptr<CCreature> BuildCreatureFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent = nullptr);
	std::shared_ptr<CCharacter> BuildCharacterFromTemplate(IRenderDevice& RenderDevice, IScene& Scene, const SCharacterVisualTemplate& CharacterVisualTemplate, const std::shared_ptr<ISceneNode>& Parent = nullptr);
	std::shared_ptr<CCharacter> BuildCharacterFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent = nullptr);
	std::shared_ptr<ISceneNode> BuildGameObjectFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent = nullptr);
#endif

	// IWoWObjectsCreator
#ifdef USE_M2_MODELS
	std::shared_ptr<CM2> LoadM2(IRenderDevice& RenderDevice, const std::string& Filename);
#endif

#ifdef USE_WMO_MODELS
	std::shared_ptr<CWMO> LoadWMO(IRenderDevice& RenderDevice, const std::string& Filename);
#endif


private:
#ifdef USE_M2_MODELS
	std::shared_ptr<CM2> CreateCreatureModel(IRenderDevice& RenderDevice, const DBC_CreatureDisplayInfoRecord* CreatureDisplayInfo);
	std::shared_ptr<CM2> CreateCharacterModel(IRenderDevice& RenderDevice, const SCharacterVisualTemplate& CharacterVisualTemplate);
	std::shared_ptr<CM2> CreateGameObjectM2Model(IRenderDevice& RenderDevice, const DBC_GameObjectDisplayInfoRecord* GameObjectDisplayInfoRecord);
#endif

#ifdef USE_WMO_MODELS
	std::shared_ptr<CWMO> CreateGameObjectWMOModel(IRenderDevice& RenderDevice, const DBC_GameObjectDisplayInfoRecord* GameObjectDisplayInfoRecord);
#endif


private:
	CWorldClient& m_WorldClient;
	const IBaseManager& m_BaseManager;
	CDBCStorage* m_DBCs;

#ifdef USE_M2_MODELS
	std::mutex m_M2Lock;
	std::unordered_map<std::string, std::weak_ptr<CM2>> m_M2ObjectsWPtrs;
#endif

#ifdef USE_WMO_MODELS
	std::mutex m_WMOLock;
	std::unordered_map<std::string, std::weak_ptr<CWMO>> m_WMOObjectsWPtrs;
#endif
};
