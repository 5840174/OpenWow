#include "stdafx.h"

// Include 
#include "../WorldSocket/WorldSocket.h"

// General
#include "Warden.h"

// Additional
#include "AuthSocket/Cryptography/SHA1Randx.h"
#include "AuthSocket/Cryptography/HMACSHA1.h"

#include "WorldSocket/ClientPacket.h"
#include "WardenModuleWin.h"

// Additional (OpenSSL)
#include <openssl/md5.h>

CWarden::CWarden(CWorldSocket& WorldSocket, BigNumber Key)
	: m_WorldSocket(WorldSocket)
	, _inputCrypto(16)
	, _outputCrypto(16)
	, m_WARDEN_SMSG_MODULE_USE_DataSize(0)
	, m_WARDEN_SMSG_MODULE_CACHE_DataPosition(0)
{
	SHA1Randx WK(Key.AsByteArray().get(), Key.GetNumBytes());
	WK.Generate(_inputKey, 16);
	WK.Generate(_outputKey, 16);
	std::memcpy(_seed, Module.Seed, 16);

	_inputCrypto.Init(_inputKey);
	_outputCrypto.Init(_outputKey);

	BigNumber bnnn;
	bnnn.SetHexStr("CC8951AFE76571513777AA97140E890236C50602");

	for (size_t i = 0; i < bnnn.GetNumBytes(); i++)
	{
		Log::Error("Byte '%d'.", bnnn.AsByteArray()[i]);
	}
}


ClientWardenModule* CWarden::GetModuleForClient()
{
	ClientWardenModule *mod = new ClientWardenModule;

	uint32 length = sizeof(Module.Module);

	// data assign
	mod->CompressedSize = length;
	mod->CompressedData = new uint8[length];
	memcpy(mod->CompressedData, Module.Module, length);
	memcpy(mod->Key, Module.ModuleKey, 16);

	// md5 hash
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, mod->CompressedData, length);
	MD5_Final((uint8*)&mod->Id, &ctx);

	return mod;
}

bool CWarden::ProcessPacket(CServerPacket& Buffer)
{
	DecryptData(Buffer.getDataFromCurrentEx(), Buffer.getSize() - Buffer.getPos());

	uint8 wardenOpcode;
	Buffer >> wardenOpcode;

	switch (wardenOpcode)
	{
		case WARDEN_SMSG_MODULE_USE:
		{
			On_WARDEN_SMSG_MODULE_USE(Buffer);
		}
		break;

		case WARDEN_SMSG_MODULE_CACHE:
		{
			On_WARDEN_SMSG_MODULE_CACHE(Buffer);
		}
		break;

		case WARDEN_SMSG_CHEAT_CHECKS_REQUEST:
		{
			On_WARDEN_SMSG_CHEAT_CHECKS_REQUEST(Buffer);
		}
		break;

		case WARDEN_SMSG_MODULE_INITIALIZE:
		{
			Buffer.seekRelative(-1);

			WardenInitModuleRequest Request;
			Buffer >> Request;

			Log::Print("CWarden::ProcessPacket: WARDEN_SMSG_MODULE_INITIALIZE Received. Module initialize.");
		}
		break;

		case WARDEN_SMSG_MEM_CHECKS_REQUEST:        // byte len; while (!EOF) { byte unk(1); byte index(++); string module(can be 0); int offset; byte len; byte[] bytes_to_compare[len]; }
		{
			throw CException("Unexpected.");
		}
		break;

		case WARDEN_SMSG_HASH_REQUEST:
		{
			On_WARDEN_SMSG_HASH_REQUEST(Buffer);
		}
		break;

		default:
			throw CException("CWarden::ProcessPacket: Unknown warden packet type '%d'.", wardenOpcode);
	}

	return true;
}

void CWarden::SendWardenPacket(uint8 WardenOpcode, CByteBuffer& Bytes)
{
	CByteBuffer bytes;
	bytes << WardenOpcode;
	bytes << Bytes;
	EncryptData(bytes.getDataEx(), bytes.getSize());

	Log::Print("WARDEN PACKET SEnDED. '%d'", WardenOpcode);

	CClientPacket clientPacket(CMSG_WARDEN_DATA);
	clientPacket << bytes;
	m_WorldSocket.SendPacket(clientPacket);
}


//
// Protected
//
void CWarden::DecryptData(uint8* buffer, uint32 length)
{
	_outputCrypto.UpdateData(length, buffer);
}

void CWarden::EncryptData(uint8* buffer, uint32 length)
{
	_inputCrypto.UpdateData(length, buffer);
}

uint32 CWarden::BuildChecksum(const uint8* data, uint32 length)
{
	struct keyData
	{
		union
		{
			struct
			{
				uint8 bytes[20];
			} bytes;

			struct
			{
				uint32 ints[5];
			} ints;
		};
	} hash;

	SHA1(data, length, hash.bytes.bytes);
	uint32 checkSum = 0;
	for (uint8 i = 0; i < 5; ++i)
		checkSum = checkSum ^ hash.ints.ints[i];

	return checkSum;
}




//
// Private Receive
//
void CWarden::On_WARDEN_SMSG_MODULE_USE(CServerPacket & Buffer)
{
	uint8 ModuleId[16];
	uint8 ModuleKey[16];
	uint32 Size;

	Buffer >> ModuleId;
	Buffer >> ModuleKey;
	Buffer >> Size;
	Log::Print("CWarden::ProcessPacket: WARDEN_SMSG_MODULE_USE Received. Size = '%d'.", Size);

	m_WARDEN_SMSG_MODULE_USE_DataSize = Size;
	m_WARDEN_SMSG_MODULE_CACHE_DataPosition = 0;

	// SEND

	CByteBuffer bytes;
	SendWardenPacket(WARDEN_CMSG_MODULE_MISSING, bytes);
}

void CWarden::On_WARDEN_SMSG_MODULE_CACHE(CServerPacket & Buffer)
{
	uint16 DataSize;
	uint8 Data[500];

	Buffer >> DataSize;
	if (DataSize > 500)
		throw CException("Data size must be less then 500.");
	Buffer.readBytes(Data, DataSize);
	m_WARDEN_SMSG_MODULE_CACHE_DataPosition += DataSize;

	Log::Print("CWarden::ProcessPacket: WARDEN_SMSG_MODULE_CACHE Received. Size = '%d'. Readed '%d' of '%d'.", DataSize, m_WARDEN_SMSG_MODULE_CACHE_DataPosition, m_WARDEN_SMSG_MODULE_USE_DataSize);

	// SEND

	if (m_WARDEN_SMSG_MODULE_CACHE_DataPosition == m_WARDEN_SMSG_MODULE_USE_DataSize)
	{
		Log::Print("CWarden::ProcessPacket: WARDEN_SMSG_MODULE_CACHE Received. All readed. Readed '%d'.", m_WARDEN_SMSG_MODULE_CACHE_DataPosition);

		CByteBuffer bytes;
		SendWardenPacket(WARDEN_CMSG_MODULE_OK, bytes);
	}
}

void CWarden::On_WARDEN_SMSG_CHEAT_CHECKS_REQUEST(CServerPacket & Buffer)
{
	// MPQ_CHECK:
	// LUA_STR_CHECK:
	// DRIVER_CHECK: 
	std::vector<std::string> strings;

	do
	{
		uint8 charOpcode;
		Buffer >> charOpcode;
		if (charOpcode == 0x00)
			break;

		Buffer.seekRelative(-1);

		uint8 strSize;
		Buffer >> strSize;

		std::string string;
		string.resize(strSize);
		Buffer.readBytes(&string[0], strSize);

		strings.push_back(string);

		Log::Print("CWarden::ProcessPacket: WARDEN_SMSG_CHEAT_CHECKS_REQUEST Received. String '%s' Size '%d'.", string.c_str(), strSize);
	} while (true);

	uint8 xorByte = _inputKey[0];

	uint8 timingCheck;
	Buffer >> timingCheck;
	timingCheck ^= xorByte;
	if (timingCheck != TIMING_CHECK)
		throw CException("TIMING_CHECK mismatchs!!! Stopping.");

	std::vector<WardenCheck> wardenChecks;

	bool isNeedExit = false;
	while (false == isNeedExit)
	{
		uint8 wdCheckOpcode;
		Buffer >> wdCheckOpcode;

		WardenCheck wd;
		wd.Type = wdCheckOpcode ^ xorByte;
		switch (wd.Type)
		{
			case MEM_CHECK:
			{
				uint8 zero;
				Buffer >> zero;
				if (zero != 0x00)
					throw CException("Zero mismatch.");

				Buffer >> uint32(wd.Address);
				Buffer >> uint8(wd.Length);
				wardenChecks.push_back(wd);

				break;
			}
			case PAGE_CHECK_A:
			case PAGE_CHECK_B:
			{
				Buffer.readBytes(wd.Data, 16);
				Buffer >> uint32(wd.Address);
				Buffer >> uint8(wd.Length);
				Buffer.seekRelative(1);

				wardenChecks.push_back(wd);

				break;
			}
			case MPQ_CHECK:
			case LUA_STR_CHECK:
			{
				uint8 index;
				Buffer >> index;
				if (index - uint8(1u) >= strings.size())
					throw CException("Incorrect string index.");
				wd.Str = strings.at(index - 1);
				wardenChecks.push_back(wd);

				break;
			}
			case DRIVER_CHECK:
			{
				Buffer.readBytes(wd.Data, 20);

				uint8 index;
				Buffer >> index;
				if (index - uint8(1u) >= strings.size())
					throw CException("Incorrect string index.");
				wd.Str = strings.at(index - 1);
				wardenChecks.push_back(wd);

				break;
			}
			case MODULE_CHECK:
			{
				uint32 seed;
				Buffer >> seed;

				HmacHash hmac(4, (uint8*)&seed);
				hmac.UpdateData(wd.Str);
				hmac.Finalize();
				Buffer.readBytes(hmac.GetDigest(), hmac.GetLength());
				wardenChecks.push_back(wd);

				break;
			}
			case PROC_CHECK:
			{
				throw CException("Unexpected.");
				/*Buffer.readBytes(wd.Data, 20);

				uint8 index0;
				Buffer >> index0;

				uint8 index1;
				Buffer >> index1;

				Buffer >> uint32(wd.Address);
				Buffer >> uint8(wd.Length);
				wardenChecks.push_back(wd);
				break;*/
			}
			default:
			{
				if (wdCheckOpcode != xorByte)
					throw CException("XOR BYTES MISMATCH.");

				isNeedExit = true;
				break;
			}
		}
	}

	Send_WARDEN_CMSG_CHEAT_CHECKS_RESULT(wardenChecks);
}

void CWarden::On_WARDEN_SMSG_HASH_REQUEST(CServerPacket & Buffer)
{
	uint8 seed[16];
	Buffer >> seed;
	if (std::memcmp(seed, _seed, 16) != 0)
		throw CException("Seeds mismatchs!!! Stopping.");

	Log::Print("CWarden::ProcessPacket: WARDEN_SMSG_HASH_REQUEST: Seed received.");

	// SEND

	CByteBuffer bytes;
	bytes.writeBytes(Module.ClientKeySeedHash, 20);
	SendWardenPacket(WARDEN_CMSG_HASH_RESULT, bytes);

	memcpy(_inputKey, Module.ClientKeySeed, 16);
	memcpy(_outputKey, Module.ServerKeySeed, 16);

	_inputCrypto.Init(Module.ClientKeySeed);
	_outputCrypto.Init(Module.ServerKeySeed);
}



//
// Private Send
//
void CWarden::Send_WARDEN_CMSG_CHEAT_CHECKS_RESULT(const std::vector<WardenCheck>& Checks)
{
	if (Checks.size() != 1)
		return;

	const auto& firstCheck = *Checks.begin();
	if (firstCheck.Str != "DBFilesClient\\Spell.dbc")
		return;


	uint8 SpellDBC[20] = { 51, 64, 56, 81, 86, 205, 150, 4, 51, 104, 255, 88, 196, 112, 80, 93, 108, 226, 212, 169 };

	CByteBuffer mpqCheckData;

	// TIMING_CHECK
	mpqCheckData << uint8(1);
	mpqCheckData << uint32(123);

	// MPQ CHECK
	mpqCheckData << uint8(0);
	mpqCheckData.writeBytes(SpellDBC, 20);

	CByteBuffer bytes;
	uint16 Length = mpqCheckData.getSize();
	bytes << Length;
	uint32 Checksum = BuildChecksum(mpqCheckData.getData(), mpqCheckData.getSize());
	bytes << Checksum;

	bytes << mpqCheckData;

	SendWardenPacket(WARDEN_CMSG_CHEAT_CHECKS_RESULT, bytes);
}
