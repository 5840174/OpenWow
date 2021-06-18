#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "Creature.h"

/*
m_FileName="Creature\\Alexstrasza\\Alexstrasza.m2"

*/

Creature::Creature(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: CM2_Base_Instance(Scene, M2Object)
{
	for (uint32 i = 0; i < MeshIDType::Count; i++)
		m_MeshID[i] = 1;
}

Creature::~Creature()
{
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

	//if (div100 == 3)
	//	return true;

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

#endif