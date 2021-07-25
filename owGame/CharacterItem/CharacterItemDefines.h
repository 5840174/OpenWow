#pragma once

#include "../M2/M2_Part_Attachment.h"

//
// Object components
//
#define MESHID_MAX_MODS 3
#define MESHID_ALLUNK { EM2GeosetType::UNK, EM2GeosetType::UNK, EM2GeosetType::UNK }
#define ATTACHS_MAX 2

struct
{
	DBCItem_EInventoryItemType	ItemType;
	const char* const			ModelsAndTexturesFolder = "";
	EM2GeosetType				Geosets[MESHID_MAX_MODS] = MESHID_ALLUNK;
	uint32						AttachmentsCount = 0;
	EM2_AttachmentPoint		    AttachmentPoint[ATTACHS_MAX] = { EM2_AttachmentPoint::Count, EM2_AttachmentPoint::Count };
}
static const ItemObjectComponents[size_t(DBCItem_EInventoryItemType::__Count)] =
{
	{ DBCItem_EInventoryItemType::NON_EQUIP, },
	{ DBCItem_EInventoryItemType::HEAD,          "Head",     MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::Helm                                             },
	{ DBCItem_EInventoryItemType::NECK                                                                                                                                                                       },
	{ DBCItem_EInventoryItemType::SHOULDERS,     "SHOULDER", MESHID_ALLUNK,                                                         2, EM2_AttachmentPoint::ShoulderLeft, EM2_AttachmentPoint::ShoulderRight },
	{ DBCItem_EInventoryItemType::BODY,          "",         { EM2GeosetType::Wristbands08, EM2GeosetType::Chest10,    EM2GeosetType::UNK }                                                                  },
	{ DBCItem_EInventoryItemType::CHEST,         "",         { EM2GeosetType::Wristbands08, EM2GeosetType::Chest10,    EM2GeosetType::Trousers13 }                                                           },
	{ DBCItem_EInventoryItemType::WAIST,         "",         { EM2GeosetType::Belt18,       EM2GeosetType::UNK,        EM2GeosetType::UNK }                                                                  },
	{ DBCItem_EInventoryItemType::LEGS,          "",         { EM2GeosetType::Pants11,      EM2GeosetType::Kneepads09, EM2GeosetType::Trousers13 }                                                           },
	{ DBCItem_EInventoryItemType::FEET,          "",         { EM2GeosetType::Boots05,      EM2GeosetType::UNK,        EM2GeosetType::UNK }                                                                  },
	{ DBCItem_EInventoryItemType::WRISTS                                                                                                                                                                     },
	{ DBCItem_EInventoryItemType::HANDS,         "",         { EM2GeosetType::Glove04,      EM2GeosetType::UNK,        EM2GeosetType::UNK }                                                                  },

	{ DBCItem_EInventoryItemType::FINGER                                                                                                                                                                     },
	{ DBCItem_EInventoryItemType::TRINKET                                                                                                                                                                    },

	{ DBCItem_EInventoryItemType::WEAPON,        "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandRight                                        },
	{ DBCItem_EInventoryItemType::SHIELD,        "Shield",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::Shield                                           },
	{ DBCItem_EInventoryItemType::RANGED,        "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandRight                                        },

	{ DBCItem_EInventoryItemType::CLOAK,         "Cape",     { EM2GeosetType::Cloak15,   EM2GeosetType::UNK,      EM2GeosetType::UNK },      1, EM2_AttachmentPoint::Back /*Cloack specific*/                },

	{ DBCItem_EInventoryItemType::TWOHWEAPON,    "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandRight                                        },
	{ DBCItem_EInventoryItemType::BAG,           "Pouch" },
	{ DBCItem_EInventoryItemType::TABARD,        "",         { EM2GeosetType::Tabard12,    EM2GeosetType::UNK,      EM2GeosetType::UNK }                                                                     },

	{ DBCItem_EInventoryItemType::ROBE                                                                                                                                                                       },

	{ DBCItem_EInventoryItemType::WEAPONMAINHAND,"WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandRight                                        },
	{ DBCItem_EInventoryItemType::WEAPONOFFHAND, "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandLeft                                         },
	{ DBCItem_EInventoryItemType::HOLDABLE                                                                                                                                                                   },

	{ DBCItem_EInventoryItemType::AMMO,          "Ammo"                                                                                                                                                      },
	{ DBCItem_EInventoryItemType::THROWN                                                                                                                                                                     },
	{ DBCItem_EInventoryItemType::RANGEDRIGHT                                                                                                                                                                },

	{ DBCItem_EInventoryItemType::QUIVER,        "Quiver"                                                                                                                                                    },
	{ DBCItem_EInventoryItemType::RELIC                                                                                                                                                                      }
};



//
// Textures names
//
struct
{
	const DBC_CharComponent_Sections list;
	const char* const ModelsAndTexturesFolder;
}
static const ItemTextureComponents[static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT)] =
{
	{ DBC_CharComponent_Sections::ARMS_UPPER,	"ARMUPPERTEXTURE" },
	{ DBC_CharComponent_Sections::ARMS_LOWER,	"ARMLOWERTEXTURE" },
	{ DBC_CharComponent_Sections::HANDS,		"HandTexture" },
	{ DBC_CharComponent_Sections::TORSO_UPPER,	"TorsoUpperTexture" },
	{ DBC_CharComponent_Sections::TORSO_LOWER,	"TorsoLowerTexture" },
	{ DBC_CharComponent_Sections::LEGS_UPPER,	"LEGUPPERTEXTURE" },
	{ DBC_CharComponent_Sections::LEGS_LOWER,	"LegLowerTexture" },
	{ DBC_CharComponent_Sections::FEET,			"FootTexture" }
};