#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWWorldObject.h"

#include "World/Creature/Creature.h"

#include "MoveSplineFlag.h"

// FORWARD BEGIN
class CWoWWorld;
struct CInet_ItemTemplate;
// FORWARD END

enum MoveFlags
{
	MOVEFLAG_NONE = 0x00000000,
	MOVEFLAG_SLIDE = 0x00000002,
	MOVEFLAG_MARCH_ON_SPOT = 0x00000004,
	MOVEFLAG_JUMP = 0x00000008,
	MOVEFLAG_WALK = 0x00000100,
	MOVEFLAG_FLY = 0x00000200,   //For dragon (+walk = glide)
	MOVEFLAG_ORIENTATION = 0x00000400,   //Fix orientation
};

enum UnitMoveType
{
	MOVE_WALK = 0,
	MOVE_RUN = 1,
	MOVE_RUN_BACK = 2,
	MOVE_SWIM = 3,
	MOVE_SWIM_BACK = 4,
	MOVE_TURN_RATE = 5,
	MOVE_FLIGHT = 6,
	MOVE_FLIGHT_BACK = 7,
	MOVE_PITCH_RATE = 8
};

enum MovementFlags : uint32
{
    MOVEMENTFLAG_NONE                  = 0x00000000,
    MOVEMENTFLAG_FORWARD               = 0x00000001,
    MOVEMENTFLAG_BACKWARD              = 0x00000002,
    MOVEMENTFLAG_STRAFE_LEFT           = 0x00000004,
    MOVEMENTFLAG_STRAFE_RIGHT          = 0x00000008,
    MOVEMENTFLAG_LEFT                  = 0x00000010,
    MOVEMENTFLAG_RIGHT                 = 0x00000020,
    MOVEMENTFLAG_PITCH_UP              = 0x00000040,
    MOVEMENTFLAG_PITCH_DOWN            = 0x00000080,
    MOVEMENTFLAG_WALKING               = 0x00000100,               // Walking
    MOVEMENTFLAG_ONTRANSPORT           = 0x00000200,               // Used for flying on some creatures
    MOVEMENTFLAG_DISABLE_GRAVITY       = 0x00000400,               // Former MOVEMENTFLAG_LEVITATING. This is used when walking is not possible.
    MOVEMENTFLAG_ROOT                  = 0x00000800,               // Must not be set along with MOVEMENTFLAG_MASK_MOVING
    MOVEMENTFLAG_FALLING               = 0x00001000,               // damage dealt on that type of falling
    MOVEMENTFLAG_FALLING_FAR           = 0x00002000,
    MOVEMENTFLAG_PENDING_STOP          = 0x00004000,
    MOVEMENTFLAG_PENDING_STRAFE_STOP   = 0x00008000,
    MOVEMENTFLAG_PENDING_FORWARD       = 0x00010000,
    MOVEMENTFLAG_PENDING_BACKWARD      = 0x00020000,
    MOVEMENTFLAG_PENDING_STRAFE_LEFT   = 0x00040000,
    MOVEMENTFLAG_PENDING_STRAFE_RIGHT  = 0x00080000,
    MOVEMENTFLAG_PENDING_ROOT          = 0x00100000,
    MOVEMENTFLAG_SWIMMING              = 0x00200000,               // appears with fly flag also
    MOVEMENTFLAG_ASCENDING             = 0x00400000,               // press "space" when flying
    MOVEMENTFLAG_DESCENDING            = 0x00800000,
    MOVEMENTFLAG_CAN_FLY               = 0x01000000,               // Appears when unit can fly AND also walk
    MOVEMENTFLAG_FLYING                = 0x02000000,               // unit is actually flying. pretty sure this is only used for players. creatures use disable_gravity
    MOVEMENTFLAG_SPLINE_ELEVATION      = 0x04000000,               // used for flight paths
    MOVEMENTFLAG_SPLINE_ENABLED        = 0x08000000,               // used for flight paths
    MOVEMENTFLAG_WATERWALKING          = 0x10000000,               // prevent unit from falling through water
    MOVEMENTFLAG_FALLING_SLOW          = 0x20000000,               // active rogue safe fall spell (passive)
    MOVEMENTFLAG_HOVER                 = 0x40000000,               // hover, cannot jump

    MOVEMENTFLAG_MASK_MOVING = MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING | MOVEMENTFLAG_SPLINE_ELEVATION,

    MOVEMENTFLAG_MASK_TURNING = MOVEMENTFLAG_LEFT | MOVEMENTFLAG_RIGHT | MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN,

    MOVEMENTFLAG_MASK_MOVING_FLY = MOVEMENTFLAG_FLYING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING,

    /// @todo if needed: add more flags to this masks that are exclusive to players
    MOVEMENTFLAG_MASK_PLAYER_ONLY = MOVEMENTFLAG_FLYING,

    /// Movement flags that have change status opcodes associated for players
    MOVEMENTFLAG_MASK_HAS_PLAYER_STATUS_OPCODE = MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_ROOT | MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_WATERWALKING | MOVEMENTFLAG_FALLING_SLOW | MOVEMENTFLAG_HOVER
};

enum MovementFlags2 : uint32
{
    MOVEMENTFLAG2_NONE                     = 0x00000000,
    MOVEMENTFLAG2_NO_STRAFE                = 0x00000001,
    MOVEMENTFLAG2_NO_JUMPING               = 0x00000002,
    MOVEMENTFLAG2_UNK3                     = 0x00000004,        // Overrides various clientside checks
    MOVEMENTFLAG2_FULL_SPEED_TURNING       = 0x00000008,
    MOVEMENTFLAG2_FULL_SPEED_PITCHING      = 0x00000010,
    MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING    = 0x00000020,
    MOVEMENTFLAG2_UNK7                     = 0x00000040,
    MOVEMENTFLAG2_UNK8                     = 0x00000080,
    MOVEMENTFLAG2_UNK9                     = 0x00000100,
    MOVEMENTFLAG2_UNK10                    = 0x00000200,
    MOVEMENTFLAG2_INTERPOLATED_MOVEMENT    = 0x00000400,
    MOVEMENTFLAG2_INTERPOLATED_TURNING     = 0x00000800,
    MOVEMENTFLAG2_INTERPOLATED_PITCHING    = 0x00001000,
    MOVEMENTFLAG2_UNK14                    = 0x00002000,
    MOVEMENTFLAG2_UNK15                    = 0x00004000,
    MOVEMENTFLAG2_UNK16                    = 0x00008000
};

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
	WoWUnit(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid);
	virtual ~WoWUnit();

	void ProcessMovementPacket(CByteBuffer& Bytes);
	void ProcessMonsterMove(CByteBuffer& Bytes);

	virtual void OnValueUpdated(uint16 index) override;
	virtual void OnValuesUpdated(const UpdateMask & Mask) override;

	uint8 GetRace() const { return m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_RACE); }
	uint32 GetRaceMask() const { return 1 << (GetRace() - 1); }
	uint8 GetClass() const { return m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_CLASS); }
	uint32 GetClassMask() const { return 1 << (GetClass() - 1); }
	uint8 GetGender() const { return m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER); }

	// Speed
	float GetSpeed(UnitMoveType MoveType) const;
	void SetSpeed(UnitMoveType MoveType, float Speed);

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

public:
	static std::shared_ptr<WoWUnit> Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWObjectGuid Guid);
	virtual void AfterCreate(IScene& Scene) override;
	virtual void Destroy() override;

	glm::vec3 DestinationPoint;


	CInet_ItemTemplate GetItemDisplayInfoIDByItemID(uint32 ItemID);

protected:
	uint32 m_MovementFlags;
	uint16 m_MovementFlagsExtra;

	// swimming/flying
	float m_Pitch;

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
