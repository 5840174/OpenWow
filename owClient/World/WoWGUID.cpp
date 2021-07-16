#include "stdafx.h"

// General
#include "WoWGUID.h"

// Additional
#include <sstream>
#include <iomanip>

CowGuid const CowGuid::Empty = CowGuid();

char const* CowGuid::GetTypeName(EWoWObjectHighGuid high)
{
	switch (high)
	{
	case EWoWObjectHighGuid::Item:         return "Item";
	case EWoWObjectHighGuid::Player:       return "Player";
	case EWoWObjectHighGuid::GameObject:   return "Gameobject";
	case EWoWObjectHighGuid::Transport:    return "Transport";
	case EWoWObjectHighGuid::Unit:         return "Creature";
	case EWoWObjectHighGuid::Pet:          return "Pet";
	case EWoWObjectHighGuid::Vehicle:      return "Vehicle";
	case EWoWObjectHighGuid::DynamicObject: return "DynObject";
	case EWoWObjectHighGuid::Corpse:       return "Corpse";
	case EWoWObjectHighGuid::Mo_Transport: return "MoTransport";
	case EWoWObjectHighGuid::Instance:     return "InstanceID";
	case EWoWObjectHighGuid::Group:        return "Group";
	default:
		return "<unknown>";
	}
}

std::string CowGuid::ToString() const
{
	std::ostringstream str;
	str << "GUID Full: 0x" << std::hex << std::setw(16) << std::setfill('0') << _guid << std::dec;
	str << " Type: " << GetTypeName();
	if (HasEntry())
		str << (IsPet() ? " Pet number: " : " Entry: ") << GetEntry() << " ";

	str << " Low: " << GetCounter();
	return str.str();
}

CowGuid CowGuid::Global(EWoWObjectHighGuid type, CounterType_t counter)
{
	return CowGuid(type, counter);
}

CowGuid CowGuid::MapSpecific(EWoWObjectHighGuid type, uint32 entry, CounterType_t counter)
{
	return CowGuid(type, entry, counter);
}


CByteBuffer& operator<<(CByteBuffer& buf, const CowGuid& guid)
{
	buf << uint64(guid.GetRawValue());
	return buf;
}

CByteBuffer& operator>>(CByteBuffer& buf, CowGuid& guid)
{
	uint64 value;
	buf.readBytes(&value, sizeof(uint64));
	guid.Set(value);
	return buf;
}