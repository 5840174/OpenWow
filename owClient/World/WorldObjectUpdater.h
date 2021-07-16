#pragma once

// FORWARD BEGIN
class CowServerWorld;
// FORWARD END

class CWorldObjectUpdater
{
public:
	CWorldObjectUpdater(CowServerWorld& WoWWorld, IScene& Scene);
	virtual ~CWorldObjectUpdater();

	void On_SMSG_COMPRESSED_UPDATE_OBJECT(CServerPacket & Buffer);
	void On_SMSG_UPDATE_OBJECT(CServerPacket& Buffer);

private:
	void ProcessUpdatePacket(CServerPacket& Packet);

private:
	CowServerWorld& m_ServerWorld;
	IScene& m_Scene;
};