#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "Units/WoWUnit.h"

// FORWARD BEGIN
class CWoWWorld;
// FORWARD END

namespace
{
enum PlayerBytesOffsets
{
	PLAYER_BYTES_OFFSET_SKIN_ID = 0,
	PLAYER_BYTES_OFFSET_FACE_ID = 1,
	PLAYER_BYTES_OFFSET_HAIR_STYLE_ID = 2,
	PLAYER_BYTES_OFFSET_HAIR_COLOR_ID = 3
};

enum PlayerBytes2Offsets
{
	PLAYER_BYTES_2_OFFSET_FACIAL_STYLE = 0,
	PLAYER_BYTES_2_OFFSET_PARTY_TYPE = 1,
	PLAYER_BYTES_2_OFFSET_BANK_BAG_SLOTS = 2,
	PLAYER_BYTES_2_OFFSET_REST_STATE = 3
};

enum PlayerBytes3Offsets
{
	PLAYER_BYTES_3_OFFSET_GENDER = 0,
	PLAYER_BYTES_3_OFFSET_INEBRIATION = 1,
	PLAYER_BYTES_3_OFFSET_PVP_TITLE = 2,
	PLAYER_BYTES_3_OFFSET_ARENA_FACTION = 3
};
}

class ZN_API WoWPlayer
	: public WoWUnit
{
public:
	WoWPlayer(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid);
	virtual ~WoWPlayer();

	virtual void OnValuesUpdated(const UpdateMask & Mask) override;

private:
	uint8 GetSkinId() const { return m_Values.GetByteValue(PLAYER_BYTES, PLAYER_BYTES_OFFSET_SKIN_ID); }
	uint8 GetFaceId() const { return m_Values.GetByteValue(PLAYER_BYTES, PLAYER_BYTES_OFFSET_FACE_ID); }
	uint8 GetHairStyleId() const { return m_Values.GetByteValue(PLAYER_BYTES, PLAYER_BYTES_OFFSET_HAIR_STYLE_ID); }
	uint8 GetHairColorId() const { return m_Values.GetByteValue(PLAYER_BYTES, PLAYER_BYTES_OFFSET_HAIR_COLOR_ID); }
	uint8 GetFacialStyle() const { return m_Values.GetByteValue(PLAYER_BYTES_2, PLAYER_BYTES_2_OFFSET_FACIAL_STYLE); }
	uint8 GetNativeGender() const { return m_Values.GetByteValue(PLAYER_BYTES_3, PLAYER_BYTES_3_OFFSET_GENDER); }

public:
	static std::shared_ptr<WoWPlayer> Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWGuid Guid);
	virtual void Destroy() override;


};

#endif