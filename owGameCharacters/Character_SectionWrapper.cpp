#include "stdafx.h"

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

std::shared_ptr<Texture> Character_SectionWrapper::getSkinTexture(const Character* _character)
{
	for (auto i : DBC_CharSections)
	{
		if (i->Get_GeneralType() == DBC_CharSections_GeneralType::Skin &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Color() == _character->GetTemplate().skin)
		{
			std::string textureName = i->Get_Texture1();
			if (textureName.empty())
			{
				break;
			}

			return _RenderDevice->CreateTexture2D(textureName);
		}
	}

	return nullptr;
}

std::shared_ptr<Texture> Character_SectionWrapper::getSkinExtraTexture(const Character* _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (i->Get_GeneralType() == DBC_CharSections_GeneralType::Skin &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Color() == _character->GetTemplate().skin)
		{
			std::string textureName = i->Get_Texture2();
			if (textureName.empty())
			{
				break;
			}

			return _RenderDevice->CreateTexture2D(textureName);
		}
	}

	return nullptr;
}

//------------------------------------------------------------
//-- Face
//------------------------------------------------------------

std::shared_ptr<Texture> Character_SectionWrapper::getFaceLowerTexture(const Character* _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Face &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Variation() == _character->GetTemplate().face &&
			i->Get_Color() == _character->GetTemplate().skin
			)
		{
			std::string textureName = i->Get_Texture1();
			if (textureName.empty())
			{
				break;
			}

			return _RenderDevice->CreateTexture2D(textureName);
		}
	}
	return nullptr;
}

std::shared_ptr<Texture> Character_SectionWrapper::getFaceUpperTexture(const Character* _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Face &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Variation() == _character->GetTemplate().face &&
			i->Get_Color() == _character->GetTemplate().skin
			)
		{
			std::string textureName = i->Get_Texture2();
			if (textureName.empty())
			{
				break;
			}

			return _RenderDevice->CreateTexture2D(textureName);
		}
	}
	return nullptr;
}

//------------------------------------------------------------
//-- FañialHair
//------------------------------------------------------------

std::string Character_SectionWrapper::getFacialHairLowerTexture(const Character* _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::FacialHair &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Color() == _character->GetTemplate().facialStyle
			)
		{
			return i->Get_Texture1();
		}
	}
	fail1(); return "";
}

std::string Character_SectionWrapper::getFacialHairUpperTexture(const Character* _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::FacialHair &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Color() == _character->GetTemplate().facialStyle
			)
		{
			return i->Get_Texture2();
		}
	}
	fail1();	return "";
}

uint32 Character_SectionWrapper::getFacial1Geoset(const Character * _character)
{
	for (auto& i : DBC_CharacterFacialHairStyles)
	{
		if (
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Variation() == _character->GetTemplate().facialStyle
			)
		{
			return i->Get_Group_01xx();
		}
	}
	return UINT32_MAX;
}

uint32 Character_SectionWrapper::getFacial2Geoset(const Character * _character)
{
	for (auto& i : DBC_CharacterFacialHairStyles)
	{
		if (
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Variation() == _character->GetTemplate().facialStyle
			)
		{
			return i->Get_Group_02xx();
		}
	}
	return UINT32_MAX;
}

uint32 Character_SectionWrapper::getFacial3Geoset(const Character * _character)
{
	for (auto& i : DBC_CharacterFacialHairStyles)
	{
		if (
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Variation() == _character->GetTemplate().facialStyle
			)
		{
			return i->Get_Group_03xx();
		}
	}
	return UINT32_MAX;
}

uint32 Character_SectionWrapper::getFacial16Geoset(const Character * _character)
{
    for (auto& i : DBC_CharacterFacialHairStyles)
    {
        if (
            i->Get_Race() == _character->GetTemplate().Race &&
            i->Get_Gender() == _character->GetTemplate().Gender &&
            i->Get_Variation() == _character->GetTemplate().facialStyle
            )
        {
            return i->Get_Group_16xx();
        }
    }
    return UINT32_MAX;
}

uint32 Character_SectionWrapper::getFacial17Geoset(const Character * _character)
{
    for (auto& i : DBC_CharacterFacialHairStyles)
    {
        if (
            i->Get_Race() == _character->GetTemplate().Race &&
            i->Get_Gender() == _character->GetTemplate().Gender &&
            i->Get_Variation() == _character->GetTemplate().facialStyle
            )
        {
            return i->Get_Group_17xx();
        }
    }
    return UINT32_MAX;
}

//------------------------------------------------------------
//-- Hair
//------------------------------------------------------------

uint32 Character_SectionWrapper::getHairGeoset(const Character* _character)
{
	for (auto& i : DBC_CharHairGeosets)
	{
		if (
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_HairType() == _character->GetTemplate().hairStyle
			)
		{
			return i->Get_Geoset();
		}
	}
	return UINT32_MAX;
}

std::shared_ptr<Texture> Character_SectionWrapper::getHairTexture(const Character* _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Hair &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Variation() == _character->GetTemplate().hairStyle &&
			i->Get_Color() == _character->GetTemplate().hairColor
			)
		{
			std::string textureName = i->Get_Texture1();
			if (textureName.empty())
				break;

			return _RenderDevice->CreateTexture2D(textureName);
		}
	}
	return nullptr;
}

std::shared_ptr<Texture> Character_SectionWrapper::getHairScalpLowerTexture(const Character* _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Hair &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Variation() == _character->GetTemplate().hairStyle &&
			i->Get_Color() == _character->GetTemplate().hairColor
			)
		{
			std::string textureName = i->Get_Texture2();
			if (textureName.empty())
				break;

			return _RenderDevice->CreateTexture2D(textureName);
		}
	}

	return nullptr;
}

std::shared_ptr<Texture> Character_SectionWrapper::getHairScalpUpperTexture(const Character* _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Hair &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Variation() == _character->GetTemplate().hairStyle &&
			i->Get_Color() == _character->GetTemplate().hairColor
			)
		{
			std::string textureName = i->Get_Texture3();
			if (textureName.empty())
				break;

			return _RenderDevice->CreateTexture2D(textureName);
		}
	}
	return nullptr;
}

//------------------------------------------------------------
//-- NAKED
//------------------------------------------------------------

std::string Character_SectionWrapper::getNakedPelvisTexture(const Character * _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Underwear &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Color() == _character->GetTemplate().skin
			)
		{
			return i->Get_Texture1();
		}
	}

	fail1(); 
	return "";
}

std::string Character_SectionWrapper::getNakedTorsoTexture(const Character * _character)
{
	for (auto& i : DBC_CharSections)
	{
		if (
			i->Get_GeneralType() == DBC_CharSections_GeneralType::Underwear &&
			i->Get_Race() == _character->GetTemplate().Race &&
			i->Get_Gender() == _character->GetTemplate().Gender &&
			i->Get_Color() == _character->GetTemplate().skin
			)
		{
			return i->Get_Texture2();
		}
	}

	fail1();
	return "";
}
