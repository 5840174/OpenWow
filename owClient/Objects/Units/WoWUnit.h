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
	float GetSpeed(UnitSpeedType MoveType) const        { return m_Speed[MoveType]; }
	void SetSpeed(UnitSpeedType MoveType, float Speed)  { m_Speed[MoveType] = Speed; }

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

	virtual void OnMounted(uint32 MountDisplayID);
	virtual void OnDismounted();

protected:


	uint32 m_MovementFlags;
	uint16 m_MovementFlagsExtra;

	float m_StrideOrPitch; // swimming/flying

	uint32 m_FallTime; // falling

	struct JumpInfo
	{
		void Reset()
		{
			zspeed = sinAngle = cosAngle = xyspeed = 0.0f;
		}

		float zspeed, sinAngle, cosAngle, xyspeed;

	} m_Jump; // jumping

	
	float m_SplineElevation; // spline


	float m_Speed[9];


	std::shared_ptr<CCreature> m_UnitModel;
	

private:

	// Mount functional
	bool m_Mount_IsMounted;
	bool m_Mount_IsDirty;
	std::shared_ptr<CCreature> m_Mount_Creature;
};

ZN_API extern float gravity;

#endif
