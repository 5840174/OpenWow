#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWWorldObject.h"
#include "Helpers/MovementHelper.h"
#include "../../Types/MoveSplineFlag.h"
#include "../Path/Path.h"

// FORWARD BEGIN
class CWoWWorld;
struct SCharacterItemTemplate;
// FORWARD END

enum UnitDynFlags
{
	UNIT_DYNFLAG_LOOTABLE = 0x0001,
	UNIT_DYNFLAG_TRACK_UNIT = 0x0002,
	UNIT_DYNFLAG_OTHER_TAGGER = 0x0004,
	UNIT_DYNFLAG_ROOTED = 0x0008,
	UNIT_DYNFLAG_SPECIALINFO = 0x0010,
	UNIT_DYNFLAG_DEAD = 0x0020
};


// FLAGS FOR VALUES
enum UnitBytes0Offsets : uint8
{
	UNIT_BYTES_0_OFFSET_RACE = 0,
	UNIT_BYTES_0_OFFSET_CLASS = 1,
	UNIT_BYTES_0_OFFSET_GENDER = 2,
	UNIT_BYTES_0_OFFSET_POWER_TYPE = 3,
};

enum UnitBytes1Offsets : uint8
{
	UNIT_BYTES_1_OFFSET_STAND_STATE = 0,
	UNIT_BYTES_1_OFFSET_PET_TALENTS = 1,
	UNIT_BYTES_1_OFFSET_VIS_FLAG = 2,
	UNIT_BYTES_1_OFFSET_ANIM_TIER = 3
};

enum UnitBytes2Offsets : uint8
{
	UNIT_BYTES_2_OFFSET_SHEATH_STATE = 0,
	UNIT_BYTES_2_OFFSET_PVP_FLAG = 1,
	UNIT_BYTES_2_OFFSET_PET_FLAGS = 2,
	UNIT_BYTES_2_OFFSET_SHAPESHIFT_FORM = 3
};


class ZN_API WoWUnit
	: public CWoWWorldObject
{
public:
	WoWUnit(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid);
	virtual ~WoWUnit();

	void ProcessMovementPacket(CServerPacket& Bytes);
	void Do_MonsterMove(CServerPacket& Bytes);

	virtual void OnValuesUpdated(const UpdateMask & Mask) override;

	uint8 GetRace() const       { return m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_RACE); }
	uint8 GetClass() const      { return m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_CLASS); }
	uint8 GetGender() const     { return m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER); }

	// Speed
	float GetSpeed(UnitMoveType MoveType) const { return m_Speed[MoveType]; }
	void SetSpeed(UnitMoveType MoveType, float Speed) { m_Speed[MoveType] = Speed; }

	// MovementFlags
	uint32 GetMovementFlags() const { return m_MovementFlags; }
	void SetMovementFlags(uint32 flag) { m_MovementFlags = flag; }
	void AddMovementFlag(uint32 flag) { m_MovementFlags |= flag; }
	void RemoveMovementFlag(uint32 flag) { m_MovementFlags &= ~flag; }
	bool HasMovementFlag(uint32 flag) const { return (m_MovementFlags & flag) != 0; }

	// MovementFlags2
	uint16 GetExtraMovementFlags() const { return m_MovementFlagsExtra; }
	void AddExtraMovementFlag(uint16 flag) { m_MovementFlagsExtra |= flag; }
	bool HasExtraMovementFlag(uint16 flag) const { return (m_MovementFlagsExtra & flag) != 0; }

	// ISceneNode
	void Update(const UpdateEventArgs& e) override;

	void ReadMovementInfoPacket(CServerPacket& Bytes);

public:
	static std::shared_ptr<WoWUnit> Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWGuid Guid);
	virtual void Destroy() override;

	std::shared_ptr<CWoWPath> m_WoWPath;

	SCharacterItemTemplate GetItemDisplayInfoIDByItemID(uint32 ItemID);

protected:
	uint32 m_MovementFlags;
	uint16 m_MovementFlagsExtra;

	// swimming/flying
	float m_StrideOrPitch;

	// falling
	uint32 m_FallTime;

	// jumping
	struct JumpInfo
	{
		void Reset()
		{
			zspeed = sinAngle = cosAngle = xyspeed = 0.0f;
		}

		float zspeed, sinAngle, cosAngle, xyspeed;

	} m_Jump;

	// spline
	float m_SplineElevation;

	float m_Speed[9];
};

#endif
