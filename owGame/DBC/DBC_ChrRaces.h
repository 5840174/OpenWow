#pragma once

#include "DBC/Core/DBC__File.h"

DBC_DEF_BEGIN(DBC_ChrRaces)

__DBC_TVALUE(uint32, Flags, 2);
__DBC_TVALUE(uint32, FactionID, 3);
__DBC_TVALUE(uint32, ExplorationID, 4);
__DBC_TVALUE(uint32, MaleModelID, 5);
__DBC_TVALUE(uint32, FemaleModelID, 6);
__DBC_STRING(ClientPrefix, 7);
__DBC_TVALUE(uint32, BaseLanguage, 8);
// Creature type      // 7 Humanoid
// ResSicknessSpellID // 15007
__DBC_TVALUE(uint32, SplashSoundID, 11);
__DBC_STRING(ClientFileString, 12);
__DBC_REF_ID(DBC_CinematicSequences, CinematicSequenceID, 13);
__DBC_TVALUE(uint32, Alliance, 14);

__DBC_LOCSTR(RaceNameNeutral, 15);
__DBC_LOCSTR(RaceNameFemale, 32);
__DBC_LOCSTR(RaceNameMale, 49);

__DBC_STRING(facialHairCustomization0, 66);
__DBC_STRING(facialHairCustomization1, 67);
__DBC_STRING(hairCustomization, 68);

__DBC_STRING(Expansion, 69);

DBC_DEF_END
