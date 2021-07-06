#pragma once

enum WardenOpcodes
{
	// Client->Server
	WARDEN_CMSG_MODULE_MISSING = 0,
	WARDEN_CMSG_MODULE_OK = 1,
	WARDEN_CMSG_CHEAT_CHECKS_RESULT = 2,
	WARDEN_CMSG_MEM_CHECKS_RESULT = 3,        // only sent if MEM_CHECK bytes doesn't match
	WARDEN_CMSG_HASH_RESULT = 4,
	WARDEN_CMSG_MODULE_FAILED = 5,        // this is sent when client failed to load uploaded module due to cache fail

	// Server->Client
	WARDEN_SMSG_MODULE_USE = 0,
	WARDEN_SMSG_MODULE_CACHE = 1,
	WARDEN_SMSG_CHEAT_CHECKS_REQUEST = 2,
	WARDEN_SMSG_MODULE_INITIALIZE = 3,
	WARDEN_SMSG_MEM_CHECKS_REQUEST = 4,        // byte len; while (!EOF) { byte unk(1); byte index(++); string module(can be 0); int offset; byte len; byte[] bytes_to_compare[len]; }
	WARDEN_SMSG_HASH_REQUEST = 5
};

enum WardenCheckType
{
	MEM_CHECK = 0xF3, // 243: byte moduleNameIndex + uint Offset + byte Len (check to ensure memory isn't modified)
	PAGE_CHECK_A = 0xB2, // 178: uint Seed + byte[20] SHA1 + uint Addr + byte Len (scans all pages for specified hash)
	PAGE_CHECK_B = 0xBF, // 191: uint Seed + byte[20] SHA1 + uint Addr + byte Len (scans only pages starts with MZ+PE headers for specified hash)
	MPQ_CHECK = 0x98, // 152: byte fileNameIndex (check to ensure MPQ file isn't modified)
	LUA_STR_CHECK = 0x8B, // 139: byte luaNameIndex (check to ensure LUA string isn't used)
	DRIVER_CHECK = 0x71, // 113: uint Seed + byte[20] SHA1 + byte driverNameIndex (check to ensure driver isn't loaded)
	TIMING_CHECK = 0x57, //  87: empty (check to ensure GetTickCount() isn't detoured)
	PROC_CHECK = 0x7E, // 126: uint Seed + byte[20] SHA1 + byte moluleNameIndex + byte procNameIndex + uint Offset + byte Len (check to ensure proc isn't detoured)
	MODULE_CHECK = 0xD9  // 217: uint Seed + byte[20] SHA1 (check to ensure module isn't injected)
};

struct WardenCheck
{
	uint8 Type;
	uint8 Data[20];
	uint32 Address;                                         // PROC_CHECK, MEM_CHECK, PAGE_CHECK
	uint8 Length;                                           // PROC_CHECK, MEM_CHECK, PAGE_CHECK
	std::string Str;                                        // LUA, MPQ, DRIVER
	std::string Comment;
	uint16 CheckId;
};


#pragma pack(push, 1)

struct WardenInitModuleRequest
{
	uint8 Command1;
	uint16 Size1;
	uint32 CheckSumm1;
	uint8 Unk1;
	uint8 Unk2;
	uint8 Type;
	uint8 String_library1;
	uint32 Function1[4];

	uint8 Command2;
	uint16 Size2;
	uint32 CheckSumm2;
	uint8 Unk3;
	uint8 Unk4;
	uint8 String_library2;
	uint32 Function2;
	uint8 Function2_set;

	uint8 Command3;
	uint16 Size3;
	uint32 CheckSumm3;
	uint8 Unk5;
	uint8 Unk6;
	uint8 String_library3;
	uint32 Function3;
	uint8 Function3_set;
};

struct WardenModuleUse
{
	uint8 Command;
	uint8 ModuleId[16];
	uint8 ModuleKey[16];
	uint32 Size;
};

struct WardenModuleTransfer
{
	uint8 Command;
	uint16 DataSize;
	uint8 Data[500];
};

struct WardenHashRequest
{
	uint8 Command;
	uint8 Seed[16];
};

#pragma pack(pop)

struct ClientWardenModule
{
	uint8 Id[16];
	uint8 Key[16];
	uint32 CompressedSize;
	uint8* CompressedData;
};