#include "stdafx.h"

// Include 
#include "../WorldSocket/WorldSocket.h"

// General
#include "Warden.h"

// Additional
#include "AuthSocket/Cryptography/SHA1Randx.h"
#include "AuthSocket/Cryptography/HMACSHA1.h"

#include "WardenModuleWin.h"

// Additional (OpenSSL)
#include <openssl/md5.h>


namespace
{
	uint32 BuildChecksum(const uint8* data, uint32 length)
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

	const char* WardenClientOpcodeToString(EWardenOpcodes Opcode)
	{
		switch (Opcode)
		{
			case WARDEN_CMSG_MODULE_MISSING: return "MODULE_MISSING";
			case WARDEN_CMSG_MODULE_OK: return "MODULE_OK";
			case WARDEN_CMSG_CHEAT_CHECKS_RESULT: return "CHEAT_CHECKS_RESULT";
			case WARDEN_CMSG_MEM_CHECKS_RESULT: return "MEM_CHECKS_RESULT";
			case WARDEN_CMSG_HASH_RESULT: return "HASH_RESULT";
			case WARDEN_CMSG_MODULE_FAILED: return "MODULE_FAILED";
		}

		return "UNKNOWN";
	}

	const char* WardenServerOpcodeToString(EWardenOpcodes Opcode)
	{
		switch (Opcode)
		{
			case WARDEN_SMSG_MODULE_USE: return "MODULE_USE";
			case WARDEN_SMSG_MODULE_CACHE: return "MODULE_CACHE";
			case WARDEN_SMSG_CHEAT_CHECKS_REQUEST: return "CHEAT_CHECKS_REQUEST";
			case WARDEN_SMSG_MODULE_INITIALIZE: return "MODULE_INITIALIZE";
			case WARDEN_SMSG_MEM_CHECKS_REQUEST: return "MEM_CHECKS_REQUEST";
			case WARDEN_SMSG_HASH_REQUEST: return "HASH_REQUEST";
		}

		return "UNKNOWN";
	}
}


CWarden::CWarden(CWorldSocket& WorldSocket, BigNumber Key)
	: m_WorldSocket(WorldSocket)
	, m_InputCrypto(16)
	, m_OutputCrypto(16)
	, m_WARDEN_SMSG_MODULE_USE_DataSize(0)
	, m_WARDEN_SMSG_MODULE_CACHE_DataPosition(0)
{
	SHA1Randx WK(Key.AsByteArray().get(), Key.GetNumBytes());
	WK.Generate(m_InputKey, 16);
	WK.Generate(m_OutputKey, 16);
	std::memcpy(m_Seed, Module.Seed, 16);

	m_InputCrypto.Init(m_InputKey);
	m_OutputCrypto.Init(m_OutputKey);
}

CWarden::~CWarden()
{}


/*SWardenClientModule* CWarden::GetModuleForClient()
{
	SWardenClientModule *mod = new SWardenClientModule;

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
}*/

bool CWarden::ProcessPacket(CServerPacket& Buffer)
{
	DecryptData(Buffer.getDataFromCurrentEx(), Buffer.getSize() - Buffer.getPos());

	EWardenOpcodes wardenOpcode;
	Buffer >> wardenOpcode;

	Log::Print("Warden: Received '%s'.", WardenServerOpcodeToString(wardenOpcode));

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
			On_WARDEN_SMSG_MODULE_INITIALIZE(Buffer);
		}
		break;

		case WARDEN_SMSG_MEM_CHECKS_REQUEST:        // byte len; while (!EOF) { byte unk(1); byte index(++); string module(can be 0); int offset; byte len; byte[] bytes_to_compare[len]; }
		{
			throw CException("Not implemented.");
		}
		break;

		case WARDEN_SMSG_HASH_REQUEST:
		{
			On_WARDEN_SMSG_HASH_REQUEST(Buffer);
		}
		break;

		default:
			throw CException("Warden: Received unknown packet '%d'.", (uint8)wardenOpcode);
	}

	return true;
}

void CWarden::SendWardenPacket(EWardenOpcodes WardenOpcode, CByteBuffer& Bytes)
{
	CByteBuffer bytes;
	bytes << WardenOpcode;
	bytes << Bytes;
	EncryptData(bytes.getDataEx(), bytes.getSize());

	Log::Print("Warden: Sending '%s'. Size '%d'.", WardenClientOpcodeToString(WardenOpcode), Bytes.getSize());

	CClientPacket clientPacket(CMSG_WARDEN_DATA);
	clientPacket << bytes;
	m_WorldSocket.SendPacket(clientPacket);
}


//
// Protected
//
void CWarden::DecryptData(uint8* buffer, uint32 length)
{
	m_OutputCrypto.UpdateData(length, buffer);
}

void CWarden::EncryptData(uint8* buffer, uint32 length)
{
	m_InputCrypto.UpdateData(length, buffer);
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

	m_WARDEN_SMSG_MODULE_USE_DataSize = Size;
	m_WARDEN_SMSG_MODULE_CACHE_DataPosition = 0;

	// SEND

	CByteBuffer bytes;
	SendWardenPacket(WARDEN_CMSG_MODULE_OK/*MISSING*/, bytes);
}

void CWarden::On_WARDEN_SMSG_MODULE_CACHE(CServerPacket & Buffer)
{
	uint16 DataSize;
	Buffer >> DataSize;
	if (DataSize > 500)
		throw CException("Warden: SMSG_MODULE_CACHE: Data size must be less then 500.");

	Buffer.seekRelative(DataSize);
	m_WARDEN_SMSG_MODULE_CACHE_DataPosition += DataSize;


	// SEND

	if (m_WARDEN_SMSG_MODULE_CACHE_DataPosition == m_WARDEN_SMSG_MODULE_USE_DataSize)
	{
		Log::Green("Warden: SMSG_MODULE_CACHE: All module readed. Readed '%d'.", m_WARDEN_SMSG_MODULE_CACHE_DataPosition);

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

		Log::Print("Warden: SMSG_CHEAT_CHECKS_REQUEST: Add string '%s' with size '%d' to cache.", string.c_str(), strSize);
	} while (true);

	uint8 xorByte = m_InputKey[0];

	uint8 timingCheck;
	Buffer >> timingCheck;
	timingCheck ^= xorByte;
	if (timingCheck != TIMING_CHECK)
		throw CException("Warden: SMSG_CHEAT_CHECKS_REQUEST: TIMING_CHECK mismatch!");

	std::vector<WardenCheck> wardenChecks;

	bool isNeedExit = false;
	while (false == isNeedExit)
	{
		uint8 wdCheckOpcode;
		Buffer >> wdCheckOpcode;

		WardenCheck wd;
		wd.Type = static_cast<EWardenCheckType>(wdCheckOpcode ^ xorByte);
		switch (wd.Type)
		{
			case MEM_CHECK:
			{
				uint8 zero;
				Buffer >> zero;
				if (zero != 0x00)
					throw CException("Warden: SMSG_CHEAT_CHECKS_REQUEST: Zero mismatch.");

				Buffer >> uint32(wd.Address);
				Buffer >> uint8(wd.Length);
				wardenChecks.push_back(wd);

				break;
			}
			case PAGE_CHECK_A:
			case PAGE_CHECK_B:
			{
				Buffer.readBytes(wd.Data, sizeof(wd.Data));
				Buffer >> uint32(wd.Address);
				Buffer >> uint8(wd.Length);

				wardenChecks.push_back(wd);

				break;
			}
			case MPQ_CHECK:
			case LUA_STR_CHECK:
			{
				uint8 index;
				Buffer >> index;
				index -= 1u;
				if (index >= strings.size())
					throw CException("Warden: SMSG_CHEAT_CHECKS_REQUEST: Incorrect string index. Index '%d'. Size '%d'", index, strings.size());

				wd.Str = strings.at(index);
				wardenChecks.push_back(wd);

				break;
			}
			case DRIVER_CHECK:
			{
				Buffer.readBytes(wd.Data, sizeof(wd.Data));

				uint8 index;
				Buffer >> index;
				index -= 1u;
				if (index >= strings.size())
					throw CException("Warden: SMSG_CHEAT_CHECKS_REQUEST: Incorrect string index. Index '%d'. Size '%d'", index, strings.size());

				wd.Str = strings.at(index);
				wardenChecks.push_back(wd);
			}
			break;

			case MODULE_CHECK:
			{
				uint32 seed;
				Buffer >> seed;

				//HmacHash hmac(4, (uint8*)&seed);
				//hmac.UpdateData(wd.Str);
				//hmac.Finalize();

				uint8 hmacHash[20];
				Buffer.readBytes(hmacHash, SHA_DIGEST_LENGTH);
				wardenChecks.push_back(wd);
			}
			break;

			case PROC_CHECK:
			{
				Buffer.readBytes(wd.Data, sizeof(wd.Data));

				uint8 index0;
				Buffer >> index0;

				uint8 index1;
				Buffer >> index1;

				Buffer >> uint32(wd.Address);
				Buffer >> uint8(wd.Length);
				wardenChecks.push_back(wd);
				
			}
			break;

			default:
			{
				if (wdCheckOpcode != xorByte)
					throw CException("Warden: SMSG_CHEAT_CHECKS_REQUEST: Control XOR byte mismatch!");

				isNeedExit = true;
				break;
			}
		}
	}

	// Send

	Send_WARDEN_CMSG_CHEAT_CHECKS_RESULT(wardenChecks);
}

void CWarden::On_WARDEN_SMSG_MODULE_INITIALIZE(CServerPacket & Buffer)
{
	Buffer.seekRelative(-1);

	SWardenInitModuleRequest Request;
	Buffer >> Request;
}

void CWarden::On_WARDEN_SMSG_HASH_REQUEST(CServerPacket & Buffer)
{
	uint8 seed[16];
	Buffer >> seed;
	if (std::memcmp(seed, m_Seed, 16) != 0)
		throw CException("Warden: SMSG_HASH_REQUEST: Modules seeds mismatch.");

	// SEND

	CByteBuffer bytes;
	bytes.writeBytes(Module.ClientKeySeedHash, 20);
	SendWardenPacket(WARDEN_CMSG_HASH_RESULT, bytes);

	std::memcpy(m_InputKey, Module.ClientKeySeed, 16);
	std::memcpy(m_OutputKey, Module.ServerKeySeed, 16);

	m_InputCrypto.Init(m_InputKey);
	m_OutputCrypto.Init(m_OutputKey);
}



//
// Private Send
//
void CWarden::Send_WARDEN_CMSG_CHEAT_CHECKS_RESULT(const std::vector<WardenCheck>& Checks)
{
	if (Checks.empty())
		return;

	for (const auto& checksIt : Checks)
	{
		std::string dataHex;
		for (size_t i = 0; i < sizeof(checksIt.Data); i++)
		{
			char dataHexChar[3];
			sprintf(dataHexChar, "%02X", checksIt.Data[i]);
			dataHex += dataHexChar;
		}

		try
		{
			switch (checksIt.Type)
			{
				case PAGE_CHECK_A:
				case PAGE_CHECK_B:
				case DRIVER_CHECK:
				case MODULE_CHECK:
				case LUA_STR_CHECK:
				case PROC_CHECK:
				{

				}
				break;

				case MEM_CHECK:
				{
					Get_MEM_CHECK(checksIt);
				}
				break;

				case MPQ_CHECK:
				{
					Get_MPQ_CHECK(checksIt);
				}
				break;
			}

			//Log::Warn ("Warden: Received check: Type: '%d', Data '%s', Str '%s', Address: '%d', Length '%d'.", checksIt.Type, dataHex.c_str(), checksIt.Str.c_str(), checksIt.Address, checksIt.Length);
		}
		catch (const CException& e)
		{
			Log::Error("Warden: Received check: Type: '%d', Data '%s', Str '%s', Address: '%d', Length '%d'. -- UNKNOWN", checksIt.Type, dataHex.c_str(), checksIt.Str.c_str(), checksIt.Address, checksIt.Length);
		}
	}

	CByteBuffer resultCheckData;

	// TIMING_CHECK
	{
		resultCheckData << uint8(1);
		resultCheckData << uint32(123);
	}
	
	for (const auto& checksIt : Checks)
	{
		switch (checksIt.Type)
		{
			case PAGE_CHECK_A:
			case PAGE_CHECK_B:
			case DRIVER_CHECK:
			case MODULE_CHECK:
			{
				const uint8 byte = 0xE9; // Magic
				resultCheckData << byte;
			}
			break;

			case MEM_CHECK:
			{
				const auto& memWardenCheckWithResult = Get_MEM_CHECK(checksIt);

				BigNumber bnn;
				bnn.SetHexStr(memWardenCheckWithResult.Result);

				const uint8 memResult = 0x00; // Magic
				resultCheckData << memResult;
				resultCheckData.writeBytes(bnn.AsByteArray(0, false).get(), bnn.GetNumBytes());
			}
			break;

			case MPQ_CHECK:
			{
				const auto& mpqWardenCheckWithResult = Get_MPQ_CHECK(checksIt);

				BigNumber bnn;
				bnn.SetHexStr(mpqWardenCheckWithResult.Result);

				const uint8 mpqResult = 0x00; // Magic
				resultCheckData << mpqResult;
				resultCheckData.writeBytes(bnn.AsByteArray(0, false).get(), bnn.GetNumBytes());
			}
			break;

			case LUA_STR_CHECK:
			case PROC_CHECK:
				throw CException("Not implemented.");

			default:
				throw CException("Unexpected behaviour.");
		}
	}

	CByteBuffer bytes;
	bytes << uint16(resultCheckData.getSize());
	bytes << BuildChecksum(resultCheckData.getData(), resultCheckData.getSize());
	bytes << resultCheckData;

	SendWardenPacket(WARDEN_CMSG_CHEAT_CHECKS_RESULT, bytes);
}
