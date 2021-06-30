#pragma once

#ifdef USE_M2_MODELS

enum class MeshIDType : uint8
{
	SkinAndHair = 0,// 0: Skin // Hair {1 - 21: various hairstyles}
	Facial1,		// 01** Facial1                         : {1 - 8: varies} (usually beard, but not always)
	Facial2,		// 02** Facial2                         : {1: (DNE), 2 - 6 : varies} (usually mustache, but not always)
	Facial3,		// 03** Facial3                         : {1: (DNE), 2 - 11 : varies} (usually sideburns, but not always)
	Glove,			// 04** Glove                           : {1 - 4}
	Boots,			// 05** Boots                           : {1 - 5}
	Unk06,			// 06** Tail (Draenei Female)
	Ears,			// 07** Ears                            : {1: (DNE), 2 : ears}
	Wristbands,		// 08** Wristbands / Sleeves            : {1: (DNE), 2 : normal, 3 : ruffled}
	Kneepads,		// 09** Kneepads / Legcuffs             : {1: (DNE), 2 : long, 3 : short}
	Chest,			// 10** Chest                           : {1: (DNE), 2 : Unk (exists but purpose unknown - mesh is in all character models lower half of "Torso Lower")}
	Pants,			// 11** Pants                           : {1: regular, 2 : short skirt, 4 : armored pants}
	Tabard,			// 12** Tabard                          : {1: (DNE), 2 : tabard}
	Trousers,		// 13** Trousers                        : {1: legs, 2 : dress}
	Unk14,			// 14** 
	Cloak,			// 15** Cloak                           : {1 - 10: various cloak styles}
	Unk16,			// 16** 
	Eyeglows,		// 17** Eyeglows                        : {1: (DNE), 2 : racial eyeglow, 3 : DK eyeglow}
	Belt,			// 18** Belt / bellypack                : {1: (DNE), 2 : bulky belt}
	Tail,           // 19** Bone/Tail
	Feet,           // 20** Feet                            : {1: (DNE), 2: feet}

	UNK             = 21,
	Count           = 21
};

//07**
enum class EarsStyles : uint8
{
	None = 1,
	Enabled
};

//17**
enum class EyeglowsStyles : uint8
{
	None = 1,
	Racial,
	DK
};

#endif