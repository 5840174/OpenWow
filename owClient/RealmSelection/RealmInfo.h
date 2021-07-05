#pragma once

class SRealmInfo
{
public:
	enum RealmFlags : uint8
	{
		REALM_FLAG_NONE = 0x00,
		REALM_FLAG_VERSION_MISMATCH = 0x01,
		REALM_FLAG_OFFLINE = 0x02,
		REALM_FLAG_SPECIFYBUILD = 0x04,
		REALM_FLAG_UNK1 = 0x08,
		REALM_FLAG_UNK2 = 0x10,
		REALM_FLAG_RECOMMENDED = 0x20,
		REALM_FLAG_NEW = 0x40,
		REALM_FLAG_FULL = 0x80
	};

	enum RealmType : uint8
	{
		REALM_TYPE_NORMAL = 0,
		REALM_TYPE_PVP = 1,
		REALM_TYPE_NORMAL2 = 4,
		REALM_TYPE_RP = 6,
		REALM_TYPE_RPPVP = 8,

		MAX_CLIENT_REALM_TYPE = 14,

		REALM_TYPE_FFA_PVP = 16     // custom, free for all pvp mode like arena PvP in all zones except rest activated places and sanctuaries
	};

	struct RealmBuildInfo
	{
		uint8 MajorVersion;
		uint8 MinorVersion;
		uint8 BugfixVersion;
		uint16 Build;

		std::string toString()
		{
			return std::to_string(MajorVersion) + "." + std::to_string(MinorVersion) + "." + std::to_string(BugfixVersion) + " (" + std::to_string(Build) + ")";
		}
	};

public:
	SRealmInfo(CByteBuffer& _buff)
	{
		_buff.read(&Type);
		_buff.read(&Locked); // POST_BC_EXP_FLAG
		_buff.read(&Flag);

		_buff.readString(&Name);
		_buff.readString(&Address);
		_buff.read(&Population);
		_buff.readBytes(&CharactersCount, sizeof(uint8));
		_buff.readBytes(&TimeZone, sizeof(uint8));
		_buff.readBytes(&RealmID, sizeof(uint8)); // POST_BC_EXP_FLAG or 0

		if (Flag & REALM_FLAG_SPECIFYBUILD)
		{
			_buff.read(&BuildInfo.MajorVersion);
			_buff.read(&BuildInfo.MinorVersion);
			_buff.read(&BuildInfo.BugfixVersion);
			_buff.read(&BuildInfo.Build);
		}

		size_t pos = Address.find(':');
		_ASSERT(pos != -1);
		m_IPAddres = Address.substr(0, pos);
		m_Port = atoi(Address.substr(pos + 1).c_str());
	}

	const std::string& getIP() const { return m_IPAddres; }
	const uint16& getPort() const { return m_Port; }

	void ToString()
	{
		Log::Green("RealmType: '%d', IsLocked: '%d', Flag: '0x%X', Name: '%s', Address '%s', Population: '%f', CharacterCount: '%d'", Type, Locked, Flag, Name.c_str(), Address.c_str(), Population, CharactersCount);
	}

public:
	RealmType Type;
	uint8 Locked;              // POST_BC_EXP_FLAG
	RealmFlags Flag;
	std::string Name;
	std::string Address;
	float Population;
	uint8 CharactersCount;
	uint8 TimeZone;
	uint8 RealmID;             // POST_BC_EXP_FLAG or 0
	RealmBuildInfo BuildInfo;

private:
	std::string  m_IPAddres;
	uint16       m_Port;
};