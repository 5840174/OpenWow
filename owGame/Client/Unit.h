#pragma once

#include "WorldObject.h"

#include "World/Creature/Creature.h"

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

enum MovementFlags
{
	MOVEMENTFLAG_FORWARD = 0x1,
	MOVEMENTFLAG_BACKWARD = 0x2,
	MOVEMENTFLAG_STRAFE_LEFT = 0x4,
	MOVEMENTFLAG_STRAFE_RIGHT = 0x8,
	MOVEMENTFLAG_LEFT = 0x10,
	MOVEMENTFLAG_RIGHT = 0x20,
	MOVEMENTFLAG_PITCH_UP = 0x40,
	MOVEMENTFLAG_PITCH_DOWN = 0x80,

	MOVEMENTFLAG_WALK_MODE = 0x100,
	MOVEMENTFLAG_JUMPING = 0x2000,
	MOVEMENTFLAG_FALLING = 0x4000,
	MOVEMENTFLAG_SWIMMING = 0x200000,
	MOVEMENTFLAG_ONTRANSPORT = 0x2000000,
	MOVEMENTFLAG_SPLINE = 0x4000000,

	//[TZERO] other enumeration to delete or check  
	MOVEMENTFLAG_NONE = 0x00000000,
	MOVEMENTFLAG_LEVITATING = 0x00000400,
	MOVEMENTFLAG_FLY_UNK1 = 0x00000800,
	MOVEMENTFLAG_UNK4 = 0x00002000,
	// 0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000
	MOVEMENTFLAG_CAN_FLY = 0x00800000,
	MOVEMENTFLAG_SPLINE2 = 0x08000000,               // used for flight paths
	MOVEMENTFLAG_WATERWALKING = 0x10000000,               // prevent unit from falling through water
	MOVEMENTFLAG_SAFE_FALL = 0x20000000,               // active rogue safe fall spell (passive)
	MOVEMENTFLAG_UNK3 = 0x40000000
};

class ZN_API WoWUnit
	: public WorldObject
{
public:
	WoWUnit();
	virtual ~WoWUnit();

	virtual void UpdateMovementData(CByteBuffer& Bytes) override; // For inheritance

public:
	static std::shared_ptr<WoWUnit> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid);

protected:
	uint32 flags2;
	float Speed_MOVE_WALK = 0.0f;
	float Speed_MOVE_RUN = 0.0f;
	float Speed_MOVE_SWIM_BACK = 0.0f;
	float Speed_MOVE_SWIM = 0.0f;
	float Speed_MOVE_RUN_BACK = 0.0f;
	float Speed_MOVE_TURN_RATE = 0.0f;

private:
	std::shared_ptr<Creature> m_HiddenNode;
};