#pragma once

// FORWARD BEGIN
class CowClient;
// FORWARD END

class CowClient_CharacterSelection
{
public:
	CowClient_CharacterSelection(CowClient& WoWClient, IScene& Scene);
	virtual ~CowClient_CharacterSelection();

	void On_SMSG_CHAR_ENUM(CServerPacket& Bytes);

	void AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler);
	bool ProcessPacket(CServerPacket& Bytes);

private:
	CowClient& m_WoWClient;
	IScene& m_Scene;
	std::unordered_map<Opcodes, std::function<void(CServerPacket&)>> m_Handlers;
};