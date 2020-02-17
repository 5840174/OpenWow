#include "stdafx.h"

// General
#include "Creature.h"

/*
m_FileName="Creature\\Alexstrasza\\Alexstrasza.m2"

*/

Creature::Creature() 
	: Creature_M2Instance()
{
	for (uint32 i = 0; i < MeshIDType::Count; i++)
	{
		m_MeshID[i] = 1;
	}
}

Creature::~Creature()
{
}

void Creature::InitFromDisplayInfo(uint32 _id)
{
	const DBC_CreatureDisplayInfoRecord* rec = GetBaseManager()->GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo()[_id];
    if (rec == nullptr)
        throw std::exception("Not found!");

	const DBC_CreatureDisplayInfoExtraRecord* humanoidRecExtra = rec->Get_HumanoidData();
	_ASSERT(humanoidRecExtra == nullptr);

	// 1. Load model
	{
		CreateCreatureModel(rec);
	}

	// 2. Creature textures
	{
		if (strlen(rec->Get_Texture1()) != 0)
		{
			setSpecialTexture(SM2_Texture::Type::MONSTER_1, getM2().getFilePath() + rec->Get_Texture1() + ".blp");
		}

		if (strlen(rec->Get_Texture2()) != 0)
		{
			setSpecialTexture(SM2_Texture::Type::MONSTER_2, getM2().getFilePath() + rec->Get_Texture2() + ".blp");
		}

		if (strlen(rec->Get_Texture3()) != 0)
		{
			setSpecialTexture(SM2_Texture::Type::MONSTER_3, getM2().getFilePath() + rec->Get_Texture3() + ".blp");
		}
	}
}

void Creature::setMeshEnabled(MeshIDType::List _type, uint32 _value)
{
	_ASSERT(_type < MeshIDType::Count);
	if (_value == UINT32_MAX)
	{
		return;
	}

	m_MeshID[_type] = _value;
}

bool Creature::isMeshEnabled(uint32 _index) const
{
	uint32 div100 = _index / 100;
	uint32 mod100 = _index % 100;

	//_ASSERT(div100 < MeshIDType::Count);
	_ASSERT(div100 != 6);
	_ASSERT(div100 != 14);
	_ASSERT(div100 != 16);

	for (uint32 i = 0; i < MeshIDType::Count; i++)
	{
		// Special case for skin
		if (div100 == MeshIDType::SkinAndHair && mod100 == 0)
		{
			return true;
		}

		// Others
		if (div100 == i)
		{
			if (m_MeshID[i] == mod100)
			{
				return true;
			}
		}
	}

	return false;
}

void Creature::CreateCreatureModel(const DBC_CreatureDisplayInfoRecord* CreatureDisplayInfo)
{
	const DBC_CreatureModelDataRecord* modelRec = CreatureDisplayInfo->Get_Model();
	_ASSERT(modelRec != nullptr);

	std::string modelName = modelRec->Get_ModelPath();
	std::shared_ptr<M2> m2 = GetManager<IM2Manager>(_ApplicationInstance->GetBaseManager())->Add(modelName);
	_ASSERT(m2 != nullptr);

	setM2(m2);

	setAlpha(static_cast<float>(CreatureDisplayInfo->Get_Opacity()) / 255.0f);
	//SetScale(_record.Get_Scale());
}

