#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "Units/WoWUnit.h"

// FORWARD BEGIN
class CWorldServer;
// FORWARD END

namespace
{
enum EPlayerBytes1Offsets
{
	PLAYER_BYTES_OFFSET_SKIN_ID = 0,
	PLAYER_BYTES_OFFSET_FACE_ID = 1,
	PLAYER_BYTES_OFFSET_HAIR_STYLE_ID = 2,
	PLAYER_BYTES_OFFSET_HAIR_COLOR_ID = 3
};

enum EPlayerBytes2Offsets
{
	PLAYER_BYTES_2_OFFSET_FACIAL_STYLE = 0,
	PLAYER_BYTES_2_OFFSET_PARTY_TYPE = 1,
	PLAYER_BYTES_2_OFFSET_BANK_BAG_SLOTS = 2,
	PLAYER_BYTES_2_OFFSET_REST_STATE = 3
};

enum EPlayerBytes3Offsets
{
	PLAYER_BYTES_3_OFFSET_GENDER = 0,
	PLAYER_BYTES_3_OFFSET_INEBRIATION = 1,
	PLAYER_BYTES_3_OFFSET_PVP_TITLE = 2,
	PLAYER_BYTES_3_OFFSET_ARENA_FACTION = 3
};
}

class ZN_API CowServerPlayer
	: public CowServerUnit
{
public:
	CowServerPlayer(IScene& Scene, CWorldServer& WoWWorld, CowGuid Guid);
	virtual ~CowServerPlayer();

	virtual void OnValuesUpdated(const UpdateMask & Mask) override;

private:
	uint8 GetSkinId() const;
	uint8 GetFaceId() const;
	uint8 GetHairStyleId() const;
	uint8 GetHairColorId() const;
	uint8 GetFacialStyle() const;
	uint8 GetNativeGender() const;

public:
	static std::shared_ptr<CowServerPlayer> Create(CWorldServer& WoWWorld, IScene& Scene, CowGuid Guid);
	virtual void Destroy() override;

protected:
	virtual void                OnDisplayIDChanged(uint32 DisplayID);

protected:                      // Movement functional
	virtual void                Movement_HandlePlayerMovement(const UpdateEventArgs& e) override;
};

#endif