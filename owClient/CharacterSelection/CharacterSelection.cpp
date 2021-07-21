#include "stdafx.h"

// General
#include "CharacterSelection.h"

// Additional
#include "Client/Client.h"

CowClient_CharacterSelection::CowClient_CharacterSelection(CowClient& WoWClient, IScene & Scene)
	: m_WoWClient(WoWClient)
	, m_Scene(Scene)
{
	AddHandler(SMSG_CHAR_ENUM, std::bind(&CowClient_CharacterSelection::On_SMSG_CHAR_ENUM, this, std::placeholders::_1));
}

CowClient_CharacterSelection::~CowClient_CharacterSelection()
{}


void CowClient_CharacterSelection::AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler)
{
	_ASSERT(Handler != nullptr);
	m_Handlers.insert(std::make_pair(Opcode, Handler));
}

bool CowClient_CharacterSelection::ProcessPacket(CServerPacket& ServerPacket)
{
	const auto& handler = m_Handlers.find(ServerPacket.GetPacketOpcode());
	if (handler != m_Handlers.end())
	{
		_ASSERT(handler->second != nullptr);
		handler->second.operator()(ServerPacket);

		if (ServerPacket.getPos() != ServerPacket.getSize())
			throw CException("CowClient_CharacterSelection::ProcessPacket: Packet '%d' is not fully readed. %d of %d.", ServerPacket.GetPacketOpcode(), ServerPacket.getPos(), ServerPacket.getSize());

		return true;
	}

	return false;
}



//
// Private
//
void CowClient_CharacterSelection::On_SMSG_CHAR_ENUM(CServerPacket & Bytes)
{
	uint8 charCnt;
	Bytes >> charCnt;

	Log::Print("CowClient_CharacterSelection::On_SMSG_CHAR_ENUM:: Characters count '%d'.", charCnt);

	std::vector<SCharacterTemplate> characters;
	for (uint8 i = 0; i < charCnt; i++)
	{
		SCharacterTemplate character(Bytes);
		characters.push_back(character);

		Log::Print("CowClient_CharacterSelection::On_SMSG_CHAR_ENUM:: Character [%d] name '%s'.", i, character.Name.c_str());
	}

	if (characters.empty())
		throw CException("There is no characters.");

	m_WoWClient.OnCharactersListObtained(characters);
}