#include "stdafx.h"

// General
#include "WorldServer.h"

namespace
{
enum ChatMsg : uint8
{
	CHAT_MSG_ADDON = 0xFF, // -1
	CHAT_MSG_SYSTEM = 0x00,
	CHAT_MSG_SAY = 0x01,
	CHAT_MSG_PARTY = 0x02,
	CHAT_MSG_RAID = 0x03,
	CHAT_MSG_GUILD = 0x04,
	CHAT_MSG_OFFICER = 0x05,
	CHAT_MSG_YELL = 0x06,
	CHAT_MSG_WHISPER = 0x07,
	CHAT_MSG_WHISPER_FOREIGN = 0x08,
	CHAT_MSG_WHISPER_INFORM = 0x09,
	CHAT_MSG_EMOTE = 0x0A,
	CHAT_MSG_TEXT_EMOTE = 0x0B,
	CHAT_MSG_MONSTER_SAY = 0x0C,
	CHAT_MSG_MONSTER_PARTY = 0x0D,
	CHAT_MSG_MONSTER_YELL = 0x0E,
	CHAT_MSG_MONSTER_WHISPER = 0x0F,
	CHAT_MSG_MONSTER_EMOTE = 0x10,
	CHAT_MSG_CHANNEL = 0x11,
	CHAT_MSG_CHANNEL_JOIN = 0x12,
	CHAT_MSG_CHANNEL_LEAVE = 0x13,
	CHAT_MSG_CHANNEL_LIST = 0x14,
	CHAT_MSG_CHANNEL_NOTICE = 0x15,
	CHAT_MSG_CHANNEL_NOTICE_USER = 0x16,
	CHAT_MSG_AFK = 0x17,
	CHAT_MSG_DND = 0x18,
	CHAT_MSG_IGNORED = 0x19,
	CHAT_MSG_SKILL = 0x1A,
	CHAT_MSG_LOOT = 0x1B,
	CHAT_MSG_MONEY = 0x1C,
	CHAT_MSG_OPENING = 0x1D,
	CHAT_MSG_TRADESKILLS = 0x1E,
	CHAT_MSG_PET_INFO = 0x1F,
	CHAT_MSG_COMBAT_MISC_INFO = 0x20,
	CHAT_MSG_COMBAT_XP_GAIN = 0x21,
	CHAT_MSG_COMBAT_HONOR_GAIN = 0x22,
	CHAT_MSG_COMBAT_FACTION_CHANGE = 0x23,
	CHAT_MSG_BG_SYSTEM_NEUTRAL = 0x24,
	CHAT_MSG_BG_SYSTEM_ALLIANCE = 0x25,
	CHAT_MSG_BG_SYSTEM_HORDE = 0x26,
	CHAT_MSG_RAID_LEADER = 0x27,
	CHAT_MSG_RAID_WARNING = 0x28,
	CHAT_MSG_RAID_BOSS_EMOTE = 0x29,
	CHAT_MSG_RAID_BOSS_WHISPER = 0x2A,
	CHAT_MSG_FILTERED = 0x2B,
	CHAT_MSG_BATTLEGROUND = 0x2C,
	CHAT_MSG_BATTLEGROUND_LEADER = 0x2D,
	CHAT_MSG_RESTRICTED = 0x2E,
	CHAT_MSG_BATTLENET = 0x2F,
	CHAT_MSG_ACHIEVEMENT = 0x30,
	CHAT_MSG_GUILD_ACHIEVEMENT = 0x31,
	CHAT_MSG_ARENA_POINTS = 0x32,
	CHAT_MSG_PARTY_LEADER = 0x33
};

#define MAX_CHAT_MSG_TYPE 0x34

enum Language : int32
{
	LANG_UNIVERSAL = 0,
	LANG_ORCISH = 1,
	LANG_DARNASSIAN = 2,
	LANG_TAURAHE = 3,
	LANG_DWARVISH = 6,
	LANG_COMMON = 7,
	LANG_DEMONIC = 8,
	LANG_TITAN = 9,
	LANG_THALASSIAN = 10,
	LANG_DRACONIC = 11,
	LANG_KALIMAG = 12,
	LANG_GNOMISH = 13,
	LANG_TROLL = 14,
	LANG_GUTTERSPEAK = 33,
	LANG_DRAENEI = 35,
	LANG_ZOMBIE = 36,
	LANG_GNOMISH_BINARY = 37,
	LANG_GOBLIN_BINARY = 38,
	LANG_ADDON = (int32)0xFFFFFFFF                        // used by addons, in 2.4.0 not exist, replaced by messagetype?
};

#define LANGUAGES_COUNT   19
}

void CWorldServer::On_SMSG_EMOTE(CServerPacket & Buffer)
{
	uint32 emote;
	Buffer >> emote;
	uint64 guid;
	Buffer >> guid;
}

void CWorldServer::On_SMSG_MESSAGECHAT(CServerPacket & Buffer, bool IsGMMessage)
{
	ChatMsg chatType;
	Buffer >> chatType;

	Language language;
	Buffer >> language;

	uint64 senderGUID;
	Buffer >> senderGUID;

	Buffer.seekRelative(4);

	std::string senderNameResult;

	switch (chatType)
	{
		case CHAT_MSG_MONSTER_SAY:
		case CHAT_MSG_MONSTER_PARTY:
		case CHAT_MSG_MONSTER_YELL:
		case CHAT_MSG_MONSTER_WHISPER:
		case CHAT_MSG_MONSTER_EMOTE:
		case CHAT_MSG_RAID_BOSS_EMOTE:
		case CHAT_MSG_RAID_BOSS_WHISPER:
		case CHAT_MSG_BATTLENET:
		{
			uint32 senderNameLenght;
			Buffer >> senderNameLenght;
			std::string senderName;
			Buffer >> senderName;
			senderNameResult = senderName;

			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CowGuid receiverWoWGuid(receiverGUID);

			if (receiverWoWGuid && !receiverWoWGuid.IsPlayer() && !receiverWoWGuid.IsPet())
			{
				uint32 receiverNameLenght;
				Buffer >> receiverNameLenght;
				std::string receiverName;
				Buffer >> receiverName;
			}
		}
		break;
		case CHAT_MSG_WHISPER_FOREIGN:
		{
			uint32 senderNameLenght;
			Buffer >> senderNameLenght;
			std::string senderName;
			Buffer >> senderName;
			senderNameResult = senderName;

			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CowGuid receiverWoWGuid(receiverGUID);
		}
		break;
		case CHAT_MSG_BG_SYSTEM_NEUTRAL:
		case CHAT_MSG_BG_SYSTEM_ALLIANCE:
		case CHAT_MSG_BG_SYSTEM_HORDE:
		{
			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CowGuid receiverWoWGuid(receiverGUID);

			if (receiverWoWGuid && false == receiverWoWGuid.IsPlayer() && false == receiverWoWGuid.IsPet())
			{
				uint32 receiverNameLenght;
				Buffer >> receiverNameLenght;
				std::string receiverName;
				Buffer >> receiverName;
			}
		}
		break;
		case CHAT_MSG_ACHIEVEMENT:
		case CHAT_MSG_GUILD_ACHIEVEMENT:
		{
			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CowGuid receiverWoWGuid(receiverGUID);
		}
		break;
		default:
		{
			if (IsGMMessage)
			{
				uint32 senderNameLenght;
				Buffer >> senderNameLenght;
				std::string senderName;
				Buffer >> senderName;
				senderNameResult = senderName;
			}

			if (chatType == CHAT_MSG_CHANNEL)
			{
				std::string channelName;
				Buffer >> channelName;
			}

			uint64 receiverGUID;
			Buffer >> receiverGUID;
			CowGuid receiverWoWGuid(receiverGUID);
		}
		break;
	}

	uint32 messageLenght;
	Buffer >> messageLenght;

	std::string message;
	Buffer >> message;

	uint8 chatTag;
	Buffer >> chatTag;

	if (chatType == CHAT_MSG_ACHIEVEMENT || chatType == CHAT_MSG_GUILD_ACHIEVEMENT)
	{
		uint32 achivementID;
		Buffer >> achivementID;
	}

	Log::Print("[%s]: %s", senderNameResult.c_str(), message.c_str());

	Buffer.seek(Buffer.getSize());
}