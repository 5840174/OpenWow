#pragma once

// FORWARD BEGIN
class CWoWWorld;
// FORWARD END

class CWorldObjectUpdater
{
public:
	CWorldObjectUpdater(CWoWWorld& WoWWorld, IScene& Scene);
	virtual ~CWorldObjectUpdater();

	void On_SMSG_COMPRESSED_UPDATE_OBJECT(CServerPacket & Buffer);
	void On_SMSG_UPDATE_OBJECT(CServerPacket& Buffer);

private:
	void ProcessUpdatePacket(CServerPacket& Packet);

private:
	CWoWWorld& m_WoWWorld;
	IScene& m_Scene;
};