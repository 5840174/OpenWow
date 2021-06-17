#pragma once

#if 0

#include "WoWWorldObject.h"

#include "World/Creature/Creature.h"

#include "MoveSplineFlag.h"

enum ZN_API CreatureType : uint32
{
	CREATURE_TYPE_BEAST = 1,
	CREATURE_TYPE_DRAGONKIN = 2,
	CREATURE_TYPE_DEMON = 3,
	CREATURE_TYPE_ELEMENTAL = 4,
	CREATURE_TYPE_GIANT = 5,
	CREATURE_TYPE_UNDEAD = 6,
	CREATURE_TYPE_HUMANOID = 7,
	CREATURE_TYPE_CRITTER = 8,
	CREATURE_TYPE_MECHANICAL = 9,
	CREATURE_TYPE_NOT_SPECIFIED = 10,
	CREATURE_TYPE_TOTEM = 11,
	CREATURE_TYPE_NON_COMBAT_PET = 12,
	CREATURE_TYPE_GAS_CLOUD = 13
};

enum ZN_API CreatureTypeFlags : uint32
{
	CREATURE_TYPEFLAGS_TAMEABLE = 0x0001,
	CREATURE_TYPEFLAGS_HERBLOOT = 0x0100,
	CREATURE_TYPEFLAGS_MININGLOOT = 0x0200
};

enum ZN_API CreatureFamily : uint32
{
	CREATURE_FAMILY_WOLF = 1,
	CREATURE_FAMILY_CAT = 2,
	CREATURE_FAMILY_SPIDER = 3,
	CREATURE_FAMILY_BEAR = 4,
	CREATURE_FAMILY_BOAR = 5,
	CREATURE_FAMILY_CROCOLISK = 6,
	CREATURE_FAMILY_CARRION_BIRD = 7,
	CREATURE_FAMILY_CRAB = 8,
	CREATURE_FAMILY_GORILLA = 9,
	CREATURE_FAMILY_RAPTOR = 11,
	CREATURE_FAMILY_TALLSTRIDER = 12,
	CREATURE_FAMILY_FELHUNTER = 15,
	CREATURE_FAMILY_VOIDWALKER = 16,
	CREATURE_FAMILY_SUCCUBUS = 17,
	CREATURE_FAMILY_DOOMGUARD = 19,
	CREATURE_FAMILY_SCORPID = 20,
	CREATURE_FAMILY_TURTLE = 21,
	CREATURE_FAMILY_IMP = 23,
	CREATURE_FAMILY_BAT = 24,
	CREATURE_FAMILY_HYENA = 25,
	CREATURE_FAMILY_OWL = 26,
	CREATURE_FAMILY_WIND_SERPENT = 27,
	// [TZERO] tbc enumerations [?]
	CREATURE_FAMILY_REMOTE_CONTROL = 28,
	CREATURE_FAMILY_FELGUARD = 29,
	CREATURE_FAMILY_DRAGONHAWK = 30,
	CREATURE_FAMILY_RAVAGER = 31,
	CREATURE_FAMILY_WARP_STALKER = 32,
	CREATURE_FAMILY_SPOREBAT = 33,
	CREATURE_FAMILY_NETHER_RAY = 34,
	CREATURE_FAMILY_SERPENT = 35,
	CREATURE_FAMILY_SEA_LION = 36
};

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
	MOVEMENTFLAG_NONE = 0x00000000,
	MOVEMENTFLAG_FORWARD = 0x00000001,
	MOVEMENTFLAG_BACKWARD = 0x00000002,
	MOVEMENTFLAG_STRAFE_LEFT = 0x00000004,
	MOVEMENTFLAG_STRAFE_RIGHT = 0x00000008,
	MOVEMENTFLAG_LEFT = 0x00000010,
	MOVEMENTFLAG_RIGHT = 0x00000020,
	MOVEMENTFLAG_PITCH_UP = 0x00000040,
	MOVEMENTFLAG_PITCH_DOWN = 0x00000080,
	MOVEMENTFLAG_WALKING = 0x00000100,               // Walking
	MOVEMENTFLAG_ONTRANSPORT = 0x00000200,               // Used for flying on some creatures
	MOVEMENTFLAG_DISABLE_GRAVITY = 0x00000400,               // Former MOVEMENTFLAG_LEVITATING. This is used when walking is not possible.
	MOVEMENTFLAG_ROOT = 0x00000800,               // Must not be set along with MOVEMENTFLAG_MASK_MOVING
	MOVEMENTFLAG_FALLING = 0x00001000,               // damage dealt on that type of falling
	MOVEMENTFLAG_FALLING_FAR = 0x00002000,
	MOVEMENTFLAG_PENDING_STOP = 0x00004000,
	MOVEMENTFLAG_PENDING_STRAFE_STOP = 0x00008000,
	MOVEMENTFLAG_PENDING_FORWARD = 0x00010000,
	MOVEMENTFLAG_PENDING_BACKWARD = 0x00020000,
	MOVEMENTFLAG_PENDING_STRAFE_LEFT = 0x00040000,
	MOVEMENTFLAG_PENDING_STRAFE_RIGHT = 0x00080000,
	MOVEMENTFLAG_PENDING_ROOT = 0x00100000,
	MOVEMENTFLAG_SWIMMING = 0x00200000,               // appears with fly flag also
	MOVEMENTFLAG_ASCENDING = 0x00400000,               // press "space" when flying
	MOVEMENTFLAG_DESCENDING = 0x00800000,
	MOVEMENTFLAG_CAN_FLY = 0x01000000,               // Appears when unit can fly AND also walk
	MOVEMENTFLAG_FLYING = 0x02000000,               // unit is actually flying. pretty sure this is only used for players. creatures use disable_gravity
	MOVEMENTFLAG_SPLINE_ELEVATION = 0x04000000,               // used for flight paths
	MOVEMENTFLAG_SPLINE_ENABLED = 0x08000000,               // used for flight paths
	MOVEMENTFLAG_WATERWALKING = 0x10000000,               // prevent unit from falling through water
	MOVEMENTFLAG_FALLING_SLOW = 0x20000000,               // active rogue safe fall spell (passive)
	MOVEMENTFLAG_HOVER = 0x40000000,               // hover, cannot jump

	MOVEMENTFLAG_MASK_MOVING =
	MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT |
	MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING |
	MOVEMENTFLAG_SPLINE_ELEVATION,

	MOVEMENTFLAG_MASK_TURNING =
	MOVEMENTFLAG_LEFT | MOVEMENTFLAG_RIGHT | MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN,

	MOVEMENTFLAG_MASK_MOVING_FLY =
	MOVEMENTFLAG_FLYING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING,

	/// @todo if needed: add more flags to this masks that are exclusive to players
	MOVEMENTFLAG_MASK_PLAYER_ONLY =
	MOVEMENTFLAG_FLYING,

	/// Movement flags that have change status opcodes associated for players
	MOVEMENTFLAG_MASK_HAS_PLAYER_STATUS_OPCODE = MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_ROOT |
	MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_WATERWALKING | MOVEMENTFLAG_FALLING_SLOW | MOVEMENTFLAG_HOVER
};

enum MovementFlags2 : uint32
{
	MOVEMENTFLAG2_NONE = 0x00000000,
	MOVEMENTFLAG2_NO_STRAFE = 0x00000001,
	MOVEMENTFLAG2_NO_JUMPING = 0x00000002,
	MOVEMENTFLAG2_UNK3 = 0x00000004,        // Overrides various clientside checks
	MOVEMENTFLAG2_FULL_SPEED_TURNING = 0x00000008,
	MOVEMENTFLAG2_FULL_SPEED_PITCHING = 0x00000010,
	MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING = 0x00000020,
	MOVEMENTFLAG2_UNK7 = 0x00000040,
	MOVEMENTFLAG2_UNK8 = 0x00000080,
	MOVEMENTFLAG2_UNK9 = 0x00000100,
	MOVEMENTFLAG2_UNK10 = 0x00000200,
	MOVEMENTFLAG2_INTERPOLATED_MOVEMENT = 0x00000400,
	MOVEMENTFLAG2_INTERPOLATED_TURNING = 0x00000800,
	MOVEMENTFLAG2_INTERPOLATED_PITCHING = 0x00001000,
	MOVEMENTFLAG2_UNK14 = 0x00002000,
	MOVEMENTFLAG2_UNK15 = 0x00004000,
	MOVEMENTFLAG2_UNK16 = 0x00008000
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

class ZN_API WoWUnit
	: public WorldObject
{
public:
	WoWUnit(ObjectGuid Guid);
	virtual ~WoWUnit();

	virtual void ProcessMovementPacket(CByteBuffer& Bytes);

	float GetSpeed(UnitMoveType MoveType) const;
	void SetSpeed(UnitMoveType MoveType, float Speed);

	uint32 GetMovementFlags() const { return m_MovementFlags; }
	void SetMovementFlags(uint32 flag) { m_MovementFlags = flag; }
	void AddMovementFlag(uint32 flag) { m_MovementFlags |= flag; }
	void RemoveMovementFlag(uint32 flag) { m_MovementFlags &= ~flag; }
	bool HasMovementFlag(uint32 flag) const { return (m_MovementFlags & flag) != 0; }

	uint16 GetExtraMovementFlags() const { return m_MovementFlagsExtra; }
	void AddExtraMovementFlag(uint16 flag) { m_MovementFlagsExtra |= flag; }
	bool HasExtraMovementFlag(uint16 flag) const { return (m_MovementFlagsExtra & flag) != 0; }

public:
	static std::shared_ptr<WoWUnit> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, ObjectGuid Guid);
	virtual void AfterCreate(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene) override;

protected:
	uint32 m_MovementFlags;
	uint16 m_MovementFlagsExtra;
	
	// transport
	struct TransportInfo
	{
		void Reset()
		{
			guid = 0x00;
			pos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			seat = -1;
			time = 0;
			time2 = 0;
		}

		uint64 guid;
		glm::vec4 pos;
		int8 seat;
		uint32 time;
		uint32 time2;
	} m_Transport;

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

private:
	std::shared_ptr<Creature> m_HiddenNode;
};

#endif
