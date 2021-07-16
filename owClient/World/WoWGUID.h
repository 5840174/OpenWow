#pragma once

#define UI64LIT(N) UINT64_C(N)

enum class EWoWObjectHighGuid : uint16
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

enum ZN_API EWoWObjectTypeID : uint8
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


class CowGuid
{
public:
	static const CowGuid Empty;

	typedef uint32 EntryType_t;
	typedef uint32 CounterType_t;

	CowGuid()
		: _guid(0)
	{ }
	explicit CowGuid(uint64 guid)
		: _guid(guid)
	{ }
	explicit CowGuid(EWoWObjectHighGuid hi, EntryType_t entry, CounterType_t counter)
		: _guid(counter ? uint64(counter) | (uint64(entry) << 24) | (uint64(hi) << 48) : 0)
	{ }
	explicit CowGuid(EWoWObjectHighGuid hi, CounterType_t counter)
		: _guid(counter ? uint64(counter) | (uint64(hi) << 48) : 0)
	{ }

	operator uint64() const { return _guid; }
	void Set(uint64 guid) { _guid = guid; }
	void Clear() { _guid = 0; }

	uint64   GetRawValue() const { return _guid; }
	EWoWObjectHighGuid GetHigh() const { return EWoWObjectHighGuid((_guid >> 48) & 0x0000FFFF); }
	EntryType_t   GetEntry() const { return HasEntry() ? EntryType_t((_guid >> 24) & UI64LIT(0x0000000000FFFFFF)) : 0; }
	CounterType_t  GetCounter()  const { return HasEntry() ? CounterType_t(_guid & UI64LIT(0x0000000000FFFFFF)) : CounterType_t(_guid & UI64LIT(0x00000000FFFFFFFF)); }
	static CounterType_t GetMaxCounter(EWoWObjectHighGuid high) { return HasEntry(high) ? CounterType_t(0x00FFFFFF) : CounterType_t(0xFFFFFFFF); }
	CowGuid::CounterType_t GetMaxCounter() const { return GetMaxCounter(GetHigh()); }

	bool IsEmpty()             const { return _guid == 0; }
	bool IsCreature()          const { return GetHigh() == EWoWObjectHighGuid::Unit; }
	bool IsPet()               const { return GetHigh() == EWoWObjectHighGuid::Pet; }
	bool IsVehicle()           const { return GetHigh() == EWoWObjectHighGuid::Vehicle; }
	bool IsCreatureOrPet()     const { return IsCreature() || IsPet(); }
	bool IsCreatureOrVehicle() const { return IsCreature() || IsVehicle(); }
	bool IsAnyTypeCreature()   const { return IsCreature() || IsPet() || IsVehicle(); }
	bool IsPlayer()            const { return !IsEmpty() && GetHigh() == EWoWObjectHighGuid::Player; }
	bool IsUnit()              const { return IsAnyTypeCreature() || IsPlayer(); }
	bool IsItem()              const { return GetHigh() == EWoWObjectHighGuid::Item; }
	bool IsGameObject()        const { return GetHigh() == EWoWObjectHighGuid::GameObject; }
	bool IsDynamicObject()     const { return GetHigh() == EWoWObjectHighGuid::DynamicObject; }
	bool IsCorpse()            const { return GetHigh() == EWoWObjectHighGuid::Corpse; }
	bool IsTransport()         const { return GetHigh() == EWoWObjectHighGuid::Transport; }
	bool IsMOTransport()       const { return GetHigh() == EWoWObjectHighGuid::Mo_Transport; }
	bool IsAnyTypeGameObject() const { return IsGameObject() || IsTransport() || IsMOTransport(); }
	bool IsInstance()          const { return GetHigh() == EWoWObjectHighGuid::Instance; }
	bool IsGroup()             const { return GetHigh() == EWoWObjectHighGuid::Group; }

	static EWoWObjectTypeID GetTypeId(EWoWObjectHighGuid high)
	{
		switch (high)
		{
			case EWoWObjectHighGuid::Item:         return EWoWObjectTypeID::TYPEID_ITEM;
				//case EWoWObjectHighGuid::Container:    return TYPEID_CONTAINER; EWoWObjectHighGuid::Container == EWoWObjectHighGuid::Item currently
			case EWoWObjectHighGuid::Unit:         return EWoWObjectTypeID::TYPEID_UNIT;
			case EWoWObjectHighGuid::Pet:          return EWoWObjectTypeID::TYPEID_UNIT;
			case EWoWObjectHighGuid::Player:       return EWoWObjectTypeID::TYPEID_PLAYER;
			case EWoWObjectHighGuid::GameObject:   return EWoWObjectTypeID::TYPEID_GAMEOBJECT;
			case EWoWObjectHighGuid::DynamicObject: return EWoWObjectTypeID::TYPEID_DYNAMICOBJECT;
			case EWoWObjectHighGuid::Corpse:       return EWoWObjectTypeID::TYPEID_CORPSE;
			case EWoWObjectHighGuid::Mo_Transport: return EWoWObjectTypeID::TYPEID_GAMEOBJECT;
			case EWoWObjectHighGuid::Vehicle:      return EWoWObjectTypeID::TYPEID_UNIT;
				// unknown
			case EWoWObjectHighGuid::Instance:
			case EWoWObjectHighGuid::Group:
			default:                    return EWoWObjectTypeID::TYPEID_OBJECT;
		}
	}

	EWoWObjectTypeID GetTypeId() const { return GetTypeId(GetHigh()); }

	bool operator!() const { return IsEmpty(); }
	bool operator==(CowGuid const& guid) const { return GetRawValue() == guid.GetRawValue(); }
	bool operator!=(CowGuid const& guid) const { return GetRawValue() != guid.GetRawValue(); }
	bool operator< (CowGuid const& guid) const { return GetRawValue() < guid.GetRawValue(); }

	static char const* GetTypeName(EWoWObjectHighGuid high);
	char const* GetTypeName() const { return !IsEmpty() ? GetTypeName(GetHigh()) : "None"; }
	std::string ToString() const;

private:
	static bool HasEntry(EWoWObjectHighGuid high)
	{
		switch (high)
		{
		case EWoWObjectHighGuid::Item:
		case EWoWObjectHighGuid::Player:
		case EWoWObjectHighGuid::DynamicObject:
		case EWoWObjectHighGuid::Corpse:
		case EWoWObjectHighGuid::Mo_Transport:
		case EWoWObjectHighGuid::Instance:
		case EWoWObjectHighGuid::Group:
			return false;
		case EWoWObjectHighGuid::GameObject:
		case EWoWObjectHighGuid::Transport:
		case EWoWObjectHighGuid::Unit:
		case EWoWObjectHighGuid::Pet:
		case EWoWObjectHighGuid::Vehicle:
		default:
			return true;
		}
	}

	bool HasEntry() const { return HasEntry(GetHigh()); }

	static CowGuid Global(EWoWObjectHighGuid type, CounterType_t counter);
	static CowGuid MapSpecific(EWoWObjectHighGuid type, uint32 entry, CounterType_t counter);

	explicit CowGuid(uint32 const&) = delete;                 // no implementation, used to catch wrong type assignment
	CowGuid(EWoWObjectHighGuid, uint32, uint64 counter) = delete;       // no implementation, used to catch wrong type assignment
	CowGuid(EWoWObjectHighGuid, uint64 counter) = delete;               // no implementation, used to catch wrong type assignment

	uint64 _guid;
};


namespace std
{
	template<>
	struct hash<CowGuid>
	{
		size_t operator()(const CowGuid& Guid) const noexcept
		{
			return Guid.GetRawValue();
		}
	};
}


CByteBuffer& operator<<(CByteBuffer& buf, CowGuid const& guid);
CByteBuffer& operator>>(CByteBuffer& buf, CowGuid&       guid);
