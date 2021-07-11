#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "../Base/WoWWorldObject.h"
#include "WoWUnitFlags.h"
#include "../Helpers/Spline/MoveSplineFlag.h"
#include "../Helpers/Spline/Path.h"

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

	void OnHiddenNodePositionChanged() override;

	// UNIT_FIELD_BYTES_0
	Race GetRace() const;
	Class GetClass() const;
	Gender GetGender() const;
	uint8 GetPowerType() const;

	// Speed
	float GetSpeed(EUnitSpeedType MoveType) const        { return m_Movement_Speed[MoveType]; }
	void SetSpeed(EUnitSpeedType MoveType, float Speed) { m_Movement_Speed[MoveType] = Speed; }

	// MovementFlags
	uint32 GetMovementFlags() const                     { return m_MovementFlags; }
	bool HasMovementFlag(uint32 flag) const             { return (m_MovementFlags & flag) != 0; }

	// MovementFlags2
	uint16 GetExtraMovementFlags() const                { return m_MovementFlagsExtra; }
	bool HasExtraMovementFlag(uint16 flag) const        { return (m_MovementFlagsExtra & flag) != 0; }

	// ISceneNode
	void Update(const UpdateEventArgs& e);



	void ReadMovementInfoPacket(CServerPacket& Bytes);

public:
	static std::shared_ptr<WoWUnit> Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWGuid Guid);
	virtual void Destroy() override;

	std::shared_ptr<CWoWPath> m_WoWPath;

	// HELPERS & UTILS
	SCharacterItemTemplate GetItemDisplayInfoIDByItemID(uint32 ItemID, uint32 EnchantID);


protected:
	virtual void OnDisplayIDChanged(uint32 DisplayID);
	std::shared_ptr<CCreature> DisplayID_GetModelInstance() const;
	void DisplayID_SetModelInstance(std::shared_ptr<CCreature> ModelInstance);

	virtual void OnMounted(uint32 MountDisplayID);
	virtual void OnDismounted();

protected:
	uint32 m_MovementFlags;
	uint16 m_MovementFlagsExtra;
	float m_StrideOrPitch; // swimming/flying
	uint32 m_FallTime; // falling
	float m_SplineElevation; // spline


private:                        // DisplayID and visual
	uint32                      m_DisplayID_ID;
	std::shared_ptr<CCreature>  m_DisplayID_ModelInstance;

	bool                        m_DisplayID_Scale_IsDirty;
	float                       m_DisplayID_Scale;
	

private:                        // Movement functional
	bool                        m_Movement_IsMoveNow;
	float                       m_Movement_Speed[9];

private:                        // Jump functional
	bool                        m_Jump_IsJumpingNow;

	float                       m_Jump_z_speed;
	float                       m_Jump_sinAngle;
	float                       m_Jump_cosAngle;
	float                       m_Jump_xy_speed;

	glm::vec2                   m_JumpXZ0;
	float                       m_Jump_y0;
	float                       m_Jump_t;


private:                        // Mount functional
	bool                        m_Mount_IsMounted;
	bool                        m_Mount_IsDirty;
	std::shared_ptr<CCreature>  m_Mount_ModelInstance;
};

ZN_API extern float gravity;

#endif
