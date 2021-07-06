#pragma once

#include "../AuthSocket/Cryptography/BigNumber.h"
#include "../AuthSocket/Cryptography/ARC4.h"

#include "WardenStructs.h"

#include "../WorldSocket/ServerPacket.h"

// FORWARD BEGIN
class CWorldSocket;
// FORWARD END

class CWarden
{
public:
	CWarden(CWorldSocket& WorldSocket, BigNumber Key);

	ClientWardenModule* GetModuleForClient();

	bool ProcessPacket(CServerPacket& Bytes);

private:
	void SendWardenPacket(uint8 WardenOpcode, CByteBuffer& Bytes);

private:
	void DecryptData(uint8* buffer, uint32 length);
	void EncryptData(uint8* buffer, uint32 length);
	uint32 BuildChecksum(uint8 const* data, uint32 length);

	void On_WARDEN_SMSG_MODULE_USE(CServerPacket& Buffer);
	void On_WARDEN_SMSG_MODULE_CACHE(CServerPacket& Buffer);
	void On_WARDEN_SMSG_CHEAT_CHECKS_REQUEST(CServerPacket& Buffer);
	void On_WARDEN_SMSG_HASH_REQUEST(CServerPacket& Buffer);

private:
	void Send_WARDEN_CMSG_CHEAT_CHECKS_RESULT(const std::vector<WardenCheck>& Checks);

private:
	CWorldSocket& m_WorldSocket;

	ARC4 _inputCrypto;
	ARC4 _outputCrypto;

	uint8 _inputKey[16];
	uint8 _outputKey[16];
	uint8 _seed[16];

	uint16 m_WARDEN_SMSG_MODULE_USE_DataSize;
	uint16 m_WARDEN_SMSG_MODULE_CACHE_DataPosition;
};