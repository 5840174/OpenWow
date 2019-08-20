#include <stdafx.h>

// General
#include "CharacterTemplate.h"

CInet_CharacterTemplate::CInet_CharacterTemplate() :
	// System
	GUID(0),
	Name("Default name"),
	// Race
	Race(Race::Human),
	Class(Class::Warrior),
	Gender(Gender::Male),
	// Style
	skin(0),
	face(0),
	hairStyle(0),
	hairColor(0),
	facialStyle(0),
	// Level
	Level(255),
	// Location
	ZoneId(0),
	MapId(0),
	X(0.0f),
	Y(0.0f),
	Z(0.0f),
	// Guild
	GuildId(0),
	// Flags
	Flags(CharacterFlags::CHARACTER_FLAG_NONE),
	IsFirstLogin(true),
	// Pet
	PetInfoId(0),
	PetLevel(0),
	PetFamilyId(0)
{}

void CInet_CharacterTemplate::TemplateSet(const CInet_CharacterTemplate & _o)
{
	// System
	GUID = _o.GUID;
	Name = _o.Name;
	// Race
	Race = _o.Race;
	Class = _o.Class;
	Gender = _o.Gender;
	// Style
	skin = _o.skin;
	face = _o.face;
	hairStyle = _o.hairStyle;
	hairColor = _o.hairColor;
	facialStyle = _o.facialStyle;
	// Level
	Level = _o.Level;
	// Location
	ZoneId = _o.ZoneId;
	MapId = _o.MapId;
	X = _o.X;
	Y = _o.Y;
	Z = _o.Z;
	// Guild
	GuildId = _o.GuildId;
	// Flags
	Flags = _o.Flags;
	IsFirstLogin = _o.IsFirstLogin;
	// Pet
	PetInfoId = _o.PetInfoId;
	PetLevel = _o.PetLevel;
	PetFamilyId = _o.PetFamilyId;

	// ItemsTemplates
	for (int i = 0; i < INVENTORY_SLOT_BAG_END; i++)
	{
		ItemsTemplates[i] = _o.ItemsTemplates[i];
	}
}

void CInet_CharacterTemplate::TemplateFill(CByteBuffer& b)
{
	uint32 beginSize = b.getPos();

	b.readBytes(&GUID, 8);
	b.readString(&Name);
	b.readBytes(&Race, 1);
	b.readBytes(&Class, 1);
	b.readBytes(&Gender, 1);

	b.readBytes(&skin, 1);
	b.readBytes(&face, 1);
	b.readBytes(&hairStyle, 1);
	b.readBytes(&hairColor, 1);
	b.readBytes(&facialStyle, 1);

	b.readBytes(&Level, 1);
	b.readBytes(&ZoneId, 4);
	b.readBytes(&MapId, 4);

	b.readBytes(&X, 4);
	b.readBytes(&Y, 4);
	b.readBytes(&Z, 4);

	b.readBytes(&GuildId, 4);

	b.readBytes(&Flags, 4);
	b.readBytes(&IsFirstLogin, 1);

	b.readBytes(&PetInfoId, 4);
	b.readBytes(&PetLevel, 4);
	b.readBytes(&PetFamilyId, 4);

	for (int i = 0; i < EQUIPMENT_SLOT_END; i++)
	{
		ItemsTemplates[i].TemplateFill(b);
	}
}

void CInet_CharacterTemplate::TemplatePrint()
{
	Log::Info("Char %s, -------------------------------------------------------------", Name.c_str());
	Log::Info("Lvl %d, Race %d, Class %d, Gender %d", Level, Race, Class, Gender);
	Log::Info("Zone %d, Map %d, Pos[%f, %f, %f]", ZoneId, MapId, X, Y, Z);
	Log::Info("skin %d, face %d, hairStyle %d, hairColor %d, facialStyle %d", skin, face, hairStyle, hairColor, facialStyle);
}
