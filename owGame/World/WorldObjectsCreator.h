#pragma once

#include "DBC/DBC__Storage.h"

// Factory
#include "M2/M2.h"
#include "WMO/WMO.h"
#include "World/Creature/Creature.h"
#include "World/Character/Character.h"

#include "World/GameObject/GameObjectM2.h"
#include "World/GameObject/GameObjectWMO.h"

class ZN_API CWorldObjectCreator
	: public IWoWObjectsCreator
{
public:
	CWorldObjectCreator(IBaseManager& BaseManager);

#ifdef USE_M2_MODELS
	// Factory
	std::shared_ptr<Creature> BuildCreatureFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent = nullptr);
	std::shared_ptr<Character> BuildCharacterFromTemplate(IRenderDevice& RenderDevice, IScene& Scene, const CInet_CharacterTemplate& b, const std::shared_ptr<ISceneNode>& Parent = nullptr);
	std::shared_ptr<Character> BuildCharacterFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent = nullptr);
	std::shared_ptr<Character> BuildEmptyCharacterFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent = nullptr);
	std::shared_ptr<ISceneNode> BuildGameObjectFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent = nullptr);
#endif

	// IWoWObjectsCreator
	void ClearCache() override final;
#ifdef USE_M2_MODELS
	std::shared_ptr<CM2> LoadM2(IRenderDevice& RenderDevice, const std::string& Filename, bool ImmediateLoad = false) override final;
#endif
#ifdef USE_WMO_MODELS
	std::shared_ptr<CWMO> LoadWMO(IRenderDevice& RenderDevice, const std::string& Filename, bool ImmediateLoad = false) override final;
#endif
	void                         InitEGxBlend(IRenderDevice& RenderDevice) override final;
	std::shared_ptr<IBlendState> GetEGxBlend(uint32 Index) const override final;

private:
#ifdef USE_M2_MODELS
	std::shared_ptr<CM2> CreateCreatureModel(IRenderDevice& RenderDevice, const DBC_CreatureDisplayInfoRecord* CreatureDisplayInfo);
	std::shared_ptr<CM2> CreateCharacterModel(IRenderDevice& RenderDevice, const CInet_CharacterTemplate& CharacterTemplate);
	std::shared_ptr<CM2> CreateGameObjectM2Model(IRenderDevice& RenderDevice, const DBC_GameObjectDisplayInfoRecord* GameObjectDisplayInfoRecord);
#endif

#ifdef USE_WMO_MODELS
	std::shared_ptr<CWMO> CreateGameObjectWMOModel(IRenderDevice& RenderDevice, const DBC_GameObjectDisplayInfoRecord* GameObjectDisplayInfoRecord);
#endif

	IBlendState::BlendMode GetEGxBlendMode(uint32 Index);

private:
	IBaseManager& m_BaseManager;
	CDBCStorage* m_DBCs;

#ifdef USE_M2_MODELS
	std::mutex m_M2Lock;
	std::unordered_map<std::string, std::weak_ptr<CM2>> m_M2ObjectsWPtrs;
#endif
#ifdef USE_WMO_MODELS
	std::mutex m_WMOLock;
	std::unordered_map<std::string, std::weak_ptr<CWMO>> m_WMOObjectsWPtrs;
#endif
	std::map<uint32, std::shared_ptr<IBlendState>> m_EGxBlendStates;
};
