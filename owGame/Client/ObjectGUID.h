#pragma once

#define UI64LIT(N) UINT64_C(N)

enum class HighGuid
{
    Item           = 0x4000,                      // blizz 4000
    Container      = 0x4000,                      // blizz 4000
    Player         = 0x0000,                      // blizz 0000
    GameObject     = 0xF110,                      // blizz F110
    Transport      = 0xF120,                      // blizz F120 (for GAMEOBJECT_TYPE_TRANSPORT)
    Unit           = 0xF130,                      // blizz F130
    Pet            = 0xF140,                      // blizz F140
    Vehicle        = 0xF150,                      // blizz F550
    DynamicObject  = 0xF100,                      // blizz F100
    Corpse         = 0xF101,                      // blizz F100
    Mo_Transport   = 0x1FC0,                      // blizz 1FC0 (for GAMEOBJECT_TYPE_MO_TRANSPORT)
    Instance       = 0x1F40,                      // blizz 1F40
    Group          = 0x1F50,
};

enum ZN_API ObjectTypeID : uint8
{
    TYPEID_OBJECT        = 0,
    TYPEID_ITEM          = 1,
    TYPEID_CONTAINER     = 2,
    TYPEID_UNIT          = 3,
    TYPEID_PLAYER        = 4,
    TYPEID_GAMEOBJECT    = 5,
    TYPEID_DYNAMICOBJECT = 6,
    TYPEID_CORPSE        = 7
};


class ObjectGuid
{
public:
	static const ObjectGuid Empty;

	typedef uint32 LowType;

	ObjectGuid()
		: _guid(0)
	{ }
	explicit ObjectGuid(uint64 guid)
		: _guid(guid)
	{ }
	ObjectGuid(HighGuid hi, uint32 entry, LowType counter)
		: _guid(counter ? uint64(counter) | (uint64(entry) << 24) | (uint64(hi) << 48) : 0)
	{ }
	ObjectGuid(HighGuid hi, LowType counter)
		: _guid(counter ? uint64(counter) | (uint64(hi) << 48) : 0)
	{ }

	operator uint64() const { return _guid; }
	void Set(uint64 guid) { _guid = guid; }
	void Clear() { _guid = 0; }

	uint64   GetRawValue() const { return _guid; }
	HighGuid GetHigh() const { return HighGuid((_guid >> 48) & 0x0000FFFF); }
	uint32   GetEntry() const { return HasEntry() ? uint32((_guid >> 24) & UI64LIT(0x0000000000FFFFFF)) : 0; }
	LowType  GetCounter()  const
	{
		return HasEntry()
			? LowType(_guid & UI64LIT(0x0000000000FFFFFF))
			: LowType(_guid & UI64LIT(0x00000000FFFFFFFF));
	}

	static LowType GetMaxCounter(HighGuid high)
	{
		return HasEntry(high)
			? LowType(0x00FFFFFF)
			: LowType(0xFFFFFFFF);
	}

	ObjectGuid::LowType GetMaxCounter() const { return GetMaxCounter(GetHigh()); }

	bool IsEmpty()             const { return _guid == 0; }
	bool IsCreature()          const { return GetHigh() == HighGuid::Unit; }
	bool IsPet()               const { return GetHigh() == HighGuid::Pet; }
	bool IsVehicle()           const { return GetHigh() == HighGuid::Vehicle; }
	bool IsCreatureOrPet()     const { return IsCreature() || IsPet(); }
	bool IsCreatureOrVehicle() const { return IsCreature() || IsVehicle(); }
	bool IsAnyTypeCreature()   const { return IsCreature() || IsPet() || IsVehicle(); }
	bool IsPlayer()            const { return !IsEmpty() && GetHigh() == HighGuid::Player; }
	bool IsUnit()              const { return IsAnyTypeCreature() || IsPlayer(); }
	bool IsItem()              const { return GetHigh() == HighGuid::Item; }
	bool IsGameObject()        const { return GetHigh() == HighGuid::GameObject; }
	bool IsDynamicObject()     const { return GetHigh() == HighGuid::DynamicObject; }
	bool IsCorpse()            const { return GetHigh() == HighGuid::Corpse; }
	bool IsTransport()         const { return GetHigh() == HighGuid::Transport; }
	bool IsMOTransport()       const { return GetHigh() == HighGuid::Mo_Transport; }
	bool IsAnyTypeGameObject() const { return IsGameObject() || IsTransport() || IsMOTransport(); }
	bool IsInstance()          const { return GetHigh() == HighGuid::Instance; }
	bool IsGroup()             const { return GetHigh() == HighGuid::Group; }

	static ObjectTypeID GetTypeId(HighGuid high)
	{
		switch (high)
		{
		case HighGuid::Item:         return ObjectTypeID::TYPEID_ITEM;
			//case HighGuid::Container:    return TYPEID_CONTAINER; HighGuid::Container == HighGuid::Item currently
		case HighGuid::Unit:         return ObjectTypeID::TYPEID_UNIT;
		case HighGuid::Pet:          return ObjectTypeID::TYPEID_UNIT;
		case HighGuid::Player:       return ObjectTypeID::TYPEID_PLAYER;
		case HighGuid::GameObject:   return ObjectTypeID::TYPEID_GAMEOBJECT;
		case HighGuid::DynamicObject: return ObjectTypeID::TYPEID_DYNAMICOBJECT;
		case HighGuid::Corpse:       return ObjectTypeID::TYPEID_CORPSE;
		case HighGuid::Mo_Transport: return ObjectTypeID::TYPEID_GAMEOBJECT;
		case HighGuid::Vehicle:      return ObjectTypeID::TYPEID_UNIT;
			// unknown
		case HighGuid::Instance:
		case HighGuid::Group:
		default:                    return ObjectTypeID::TYPEID_OBJECT;
		}
	}

	ObjectTypeID GetTypeId() const { return GetTypeId(GetHigh()); }

	bool operator!() const { return IsEmpty(); }
	bool operator==(ObjectGuid const& guid) const { return GetRawValue() == guid.GetRawValue(); }
	bool operator!=(ObjectGuid const& guid) const { return GetRawValue() != guid.GetRawValue(); }
	bool operator< (ObjectGuid const& guid) const { return GetRawValue() < guid.GetRawValue(); }

	static char const* GetTypeName(HighGuid high);
	char const* GetTypeName() const { return !IsEmpty() ? GetTypeName(GetHigh()) : "None"; }
	std::string ToString() const;

private:
	static bool HasEntry(HighGuid high)
	{
		switch (high)
		{
		case HighGuid::Item:
		case HighGuid::Player:
		case HighGuid::DynamicObject:
		case HighGuid::Corpse:
		case HighGuid::Mo_Transport:
		case HighGuid::Instance:
		case HighGuid::Group:
			return false;
		case HighGuid::GameObject:
		case HighGuid::Transport:
		case HighGuid::Unit:
		case HighGuid::Pet:
		case HighGuid::Vehicle:
		default:
			return true;
		}
	}

	bool HasEntry() const { return HasEntry(GetHigh()); }

	static ObjectGuid Global(HighGuid type, LowType counter);
	static ObjectGuid MapSpecific(HighGuid type, uint32 entry, LowType counter);

	explicit ObjectGuid(uint32 const&) = delete;                 // no implementation, used to catch wrong type assignment
	ObjectGuid(HighGuid, uint32, uint64 counter) = delete;       // no implementation, used to catch wrong type assignment
	ObjectGuid(HighGuid, uint64 counter) = delete;               // no implementation, used to catch wrong type assignment

	uint64 _guid;
};

namespace std
{
	template<>
	struct hash<ObjectGuid>
	{
		size_t operator()(const ObjectGuid& Guid) const noexcept
		{
			return Guid.GetRawValue();
		}
	};
}

CByteBuffer& operator<<(CByteBuffer& buf, ObjectGuid const& guid);
CByteBuffer& operator>>(CByteBuffer& buf, ObjectGuid&       guid);
