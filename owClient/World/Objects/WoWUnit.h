#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWWorldObject.h"
#include "Helpers/MovementHelper.h"
#include "WoWUnitFlags.h"
#include "../../Types/MoveSplineFlag.h"
#include "../Path/Path.h"

// FORWARD BEGIN
class CWoWWorld;
struct SCharacterItemTemplate;
// FORWARD END




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
	float GetSpeed(UnitSpeedType MoveType) const { return m_Speed[MoveType]; }
	void SetSpeed(UnitSpeedType MoveType, float Speed) { m_Speed[MoveType] = Speed; }

	// MovementFlags
	uint32 GetMovementFlags() const { return m_MovementFlags; }
	bool HasMovementFlag(uint32 flag) const { return (m_MovementFlags & flag) != 0; }

	// MovementFlags2
	uint16 GetExtraMovementFlags() const { return m_MovementFlagsExtra; }
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
