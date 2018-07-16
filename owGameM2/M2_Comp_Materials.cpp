#include "stdafx.h"

// Include
#include "M2_Builder.h"

// General
#include "M2_Comp_Materials.h"

CM2_Comp_Materials::CM2_Comp_Materials() :
	m_IsAnimTextures(false)
{
}

CM2_Comp_Materials::~CM2_Comp_Materials()
{
	ERASE_VECTOR(m_Colors);
	ERASE_VECTOR(m_Materials);
	ERASE_VECTOR(m_Textures);
	ERASE_VECTOR(m_TextureWeights);
	ERASE_VECTOR(m_TexturesTransform);
}

void CM2_Comp_Materials::calc(uint16 anim, uint32 time, uint32 globalTime)
{
	for (auto& it : m_Colors)
	{
		it->calc(anim, time, globalTime);
	}

	for (auto& it : m_TexturesTransform)
	{
		it->calc(anim, time, globalTime);
	}

	for (auto& it : m_TextureWeights)
	{
		it->calc(anim, time, globalTime);
	}
}
