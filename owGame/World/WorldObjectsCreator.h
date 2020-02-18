#pragma once

#include "DBC/DBC__Storage.h"

// Factory
#include "World/Creature/Creature.h"
#include "World/Character/Character.h"

class ZN_API CWorldObjectCreator
{
public:
	CWorldObjectCreator(IBaseManager& BaseManager, IRenderDevice& RenderDevice);

	// Factory
	std::shared_ptr<Creature> BuildCreatureFromDisplayInfo(IScene* Scene, uint32 _id);
	std::shared_ptr<Character> BuildCharactedFromDisplayInfo(IScene* Scene, uint32 _id);

private:
	std::shared_ptr<M2> CreateCreatureModel(const DBC_CreatureDisplayInfoRecord* CreatureDisplayInfo);
	std::shared_ptr<M2> CreateCharacterModel(const CInet_CharacterTemplate& CharacterTemplate);

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	CDBCStorage* m_DBCs;
	IM2Manager* m_M2Manager;
};