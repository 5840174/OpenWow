#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "Character.h"

// General
#include "Character_SectionWrapper.h"

/*
			0 -	Base Skin		1 - Face			2 - Facial Hair		3 - Hair			4 - Underwear
Type		-					FaceType			FacialHairType		HairStyle			-
Texture1	SkinTexture			FaceLowerTexture	FacialLowerTexture	HairTexture			PelvisTexture
Texture2	ExtraSkinTexture	FaceUpperTexture	FacialUpperTexture	ScalpLowerTexture	TorsoTexture
Texture3	-					-					-					ScalpUpperTexture	-
*/

Character_SectionWrapper::Character_SectionWrapper(IBaseManager & BaseManager, IRenderDevice & RenderDevice)
	: m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
{
	m_DBCs = m_BaseManager.GetManager<CDBCStorage>();
}

Character_SectionWrapper::~Character_SectionWrapper()
{
}

std::shared_ptr<ITexture> Character_SectionWrapper::getSkinTexture(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (i->Get_GeneralType() == DBC_CharSections_GeneralType::Skin &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Color() == (uint32)Character->GetTemplate().skin)
		{
			std::string textureName = i->Get_Texture1();
			if (textureName.empty())
				break;

			return m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(textureName);
		}
	}

	return nullptr;
}

std::shared_ptr<ITexture> Character_SectionWrapper::getSkinExtraTexture(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (i->Get_GeneralType() == DBC_CharSections_GeneralType::Skin &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Color() == Character->GetTemplate().skin)
		{
			std::string textureName = i->Get_Texture2();
			if (textureName.empty())
				break;

			return m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(textureName);
		}
	}

	return nullptr;
}

//------------------------------------------------------------
//-- Face
//------------------------------------------------------------

std::shared_ptr<ITexture> Character_SectionWrapper::getFaceLowerTexture(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Face &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Type() == Character->GetTemplate().face &&
			i->Get_Color() == Character->GetTemplate().skin
			)
		{
			std::string textureName = i->Get_Texture1();
			if (textureName.empty())
				break;

			return m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(textureName);
		}
	}
	return nullptr;
}

std::shared_ptr<ITexture> Character_SectionWrapper::getFaceUpperTexture(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Face &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Type() == Character->GetTemplate().face &&
			i->Get_Color() == Character->GetTemplate().skin
			)
		{
			std::string textureName = i->Get_Texture2();
			if (textureName.empty())
				break;

			return m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(textureName);
		}
	}
	return nullptr;
}

//------------------------------------------------------------
//-- FañialHair
//------------------------------------------------------------

std::string Character_SectionWrapper::getFacialHairLowerTexture(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::FacialHair &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Color() == Character->GetTemplate().facialStyle
			)
		{
			return i->Get_Texture1();
		}
	}
	_ASSERT(false);
	return "";
}

std::string Character_SectionWrapper::getFacialHairUpperTexture(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::FacialHair &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Color() == Character->GetTemplate().facialStyle
			)
		{
			return i->Get_Texture2();
		}
	}
	_ASSERT(false);	
	return "";
}

uint32 Character_SectionWrapper::getFacial01Geoset(const Character * Character) const
{
	for (const auto& i : m_DBCs->DBC_CharacterFacialHairStyles())
	{
		if (
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Variation() == Character->GetTemplate().facialStyle
			)
		{
			return i->Get_Group_01xx();
		}
	}

	//_ASSERT(FALSE);
	return UINT32_MAX;
}

uint32 Character_SectionWrapper::getFacial02Geoset(const Character * Character) const
{
	for (const auto& i : m_DBCs->DBC_CharacterFacialHairStyles())
	{
		if (
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Variation() == Character->GetTemplate().facialStyle
			)
		{
			return i->Get_Group_02xx();
		}
	}

	//_ASSERT(FALSE);
	return UINT32_MAX;
}

uint32 Character_SectionWrapper::getFacial03Geoset(const Character * Character) const
{
	for (const auto& i : m_DBCs->DBC_CharacterFacialHairStyles())
	{
		if (
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Variation() == Character->GetTemplate().facialStyle
			)
		{
			return i->Get_Group_03xx();
		}
	}

	//_ASSERT(FALSE);
	return UINT32_MAX;
}

uint32 Character_SectionWrapper::getFacial16Geoset(const Character * Character) const
{
	for (const auto& i : m_DBCs->DBC_CharacterFacialHairStyles())
	{
		if (
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Variation() == Character->GetTemplate().facialStyle
			)
		{
			return i->Get_Group_16xx();
		}
	}

	//_ASSERT(FALSE);
	return UINT32_MAX;
}

uint32 Character_SectionWrapper::getFacial17Geoset(const Character * Character) const
{
	for (const auto& i : m_DBCs->DBC_CharacterFacialHairStyles())
	{
		if (
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Variation() == Character->GetTemplate().facialStyle
			)
		{
			return i->Get_Group_17xx();
		}
	}

	//_ASSERT(FALSE);
	return UINT32_MAX;
}



//------------------------------------------------------------
//-- Hair
//------------------------------------------------------------

uint32 Character_SectionWrapper::getHairGeoset(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharHairGeosets())
	{
		if (
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_HairType() == Character->GetTemplate().hairStyle
			)
		{
			return i->Get_Geoset();
		}
	}

	_ASSERT(false);
	return UINT32_MAX;
}

uint32 Character_SectionWrapper::getHairShowScalp(const Character * Character) const
{
	for (const auto& i : m_DBCs->DBC_CharHairGeosets())
	{
		if (
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_HairType() == Character->GetTemplate().hairStyle
			)
		{
			return i->Get_Bald();
		}
	}

	return UINT32_MAX;
}

std::shared_ptr<ITexture> Character_SectionWrapper::getHairTexture(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Hair &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Type() == Character->GetTemplate().hairStyle &&
			i->Get_Color() == Character->GetTemplate().hairColor
			)
		{
			std::string textureName = i->Get_Texture1();
			if (textureName.empty())
				break;

			return m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(textureName);
		}
	}

	return nullptr;
}

std::shared_ptr<ITexture> Character_SectionWrapper::getHairScalpLowerTexture(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Hair &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Type() == Character->GetTemplate().hairStyle &&
			i->Get_Color() == Character->GetTemplate().hairColor
			)
		{
			std::string textureName = i->Get_Texture2();
			if (textureName.empty())
				break;

			return m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(textureName);
		}
	}

	_ASSERT(false);
	return nullptr;
}

std::shared_ptr<ITexture> Character_SectionWrapper::getHairScalpUpperTexture(const Character* Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Hair &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Type() == Character->GetTemplate().hairStyle &&
			i->Get_Color() == Character->GetTemplate().hairColor
			)
		{
			std::string textureName = i->Get_Texture3();
			if (textureName.empty())
				break;

			return m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(textureName);
		}
	}

	_ASSERT(false);
	return nullptr;
}

//------------------------------------------------------------
//-- NAKED
//------------------------------------------------------------

std::string Character_SectionWrapper::getNakedPelvisTexture(const Character * Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Underwear &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Color() == Character->GetTemplate().skin
			)
		{
			return i->Get_Texture1();
		}
	}

	_ASSERT(false); 
	return "";
}

std::string Character_SectionWrapper::getNakedTorsoTexture(const Character * Character) const
{
	for (const auto& i : m_DBCs->DBC_CharSections())
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Underwear &&
			i->Get_Race() == (uint32)Character->GetTemplate().Race &&
			i->Get_Gender() == (uint32)Character->GetTemplate().Gender &&
			i->Get_Color() == Character->GetTemplate().skin
			)
		{
			return i->Get_Texture2();
		}
	}

	_ASSERT(false);
	return "";
}

#endif
