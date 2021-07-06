#pragma once

#include "../AuthSocket/Cryptography/BigNumber.h"
#include "../AuthSocket/Cryptography/ARC4.h"

#include "WardenStructs.h"
#include "WardenChecks.h"

#include "../WorldSocket/ServerPacket.h"

// FORWARD BEGIN
class CWorldSocket;
// FORWARD END

class CWarden
{
public:
	CWarden(CWorldSocket& WorldSocket, BigNumber Key);
	virtual ~CWarden();

	bool ProcessPacket(CServerPacket& Bytes);

private:
	void SendWardenPacket(uint8 WardenOpcode, CByteBuffer& Bytes);

private:
	void DecryptData(uint8* buffer, uint32 length);
	void EncryptData(uint8* buffer, uint32 length);

	void On_WARDEN_SMSG_MODULE_USE(CServerPacket& Buffer);
	void On_WARDEN_SMSG_MODULE_CACHE(CServerPacket& Buffer);
	void On_WARDEN_SMSG_CHEAT_CHECKS_REQUEST(CServerPacket& Buffer);
	void On_WARDEN_SMSG_MODULE_INITIALIZE(CServerPacket& Buffer);
	void On_WARDEN_SMSG_HASH_REQUEST(CServerPacket& Buffer);

private:
	void Send_WARDEN_CMSG_CHEAT_CHECKS_RESULT(const std::vector<WardenCheck>& Checks);

private:
	CWorldSocket& m_WorldSocket;

	ARC4 m_InputCrypto;
	ARC4 m_OutputCrypto;

	uint8 m_InputKey[16];
	uint8 m_OutputKey[16];
	uint8 m_Seed[16];

	uint16 m_WARDEN_SMSG_MODULE_USE_DataSize;
	uint16 m_WARDEN_SMSG_MODULE_CACHE_DataPosition;
};