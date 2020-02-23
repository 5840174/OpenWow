#pragma once

#include "DBC/DBC__Storage.h"

// Factory
#include "M2/M2.h"
#include "WMO/WMO.h"
#include "World/Creature/Creature.h"
#include "World/Character/Character.h"

class ZN_API CWorldObjectCreator
	: public IWoWObjectsCreator
{
public:
	CWorldObjectCreator(IBaseManager& BaseManager);

	// Factory
	std::shared_ptr<Creature> BuildCreatureFromDisplayInfo(IRenderDevice& RenderDevice, IScene* Scene, uint32 _id, const std::shared_ptr<ISceneNode3D>& Parent);
	std::shared_ptr<Character> BuildCharactedFromTemplate(IRenderDevice& RenderDevice, IScene* Scene, const CInet_CharacterTemplate& b, const std::shared_ptr<ISceneNode3D>& Parent);
	std::shared_ptr<Character> BuildCharactedFromDisplayInfo(IRenderDevice& RenderDevice, IScene* Scene, uint32 _id, const std::shared_ptr<ISceneNode3D>& Parent);

	// IWoWObjectsCreator
	std::shared_ptr<M2> LoadM2(IRenderDevice& RenderDevice, const std::string& Filename) override final;
	std::shared_ptr<CWMO> LoadWMO(IRenderDevice& RenderDevice, const std::string& Filename) override final;

private:
	std::shared_ptr<M2> CreateCreatureModel(IRenderDevice& RenderDevice, const DBC_CreatureDisplayInfoRecord* CreatureDisplayInfo);
	std::shared_ptr<M2> CreateCharacterModel(IRenderDevice& RenderDevice, const CInet_CharacterTemplate& CharacterTemplate);

private:
	IBaseManager& m_BaseManager;
	CDBCStorage* m_DBCs;
	std::unordered_map<std::string, std::weak_ptr<M2>> m_M2ObjectsWPtrs;
	std::unordered_map<std::string, std::weak_ptr<CWMO>> m_WMOObjectsWPtrs;
};