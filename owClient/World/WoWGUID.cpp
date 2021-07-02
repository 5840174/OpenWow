#include "stdafx.h"

// General
#include "WoWGUID.h"

// Additional
#include <sstream>
#include <iomanip>

CWoWGuid const CWoWGuid::Empty = CWoWGuid();

char const* CWoWGuid::GetTypeName(EWoWObjectHighGuid high)
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

std::string CWoWGuid::ToString() const
{
	std::ostringstream str;
	str << "GUID Full: 0x" << std::hex << std::setw(16) << std::setfill('0') << _guid << std::dec;
	str << " Type: " << GetTypeName();
	if (HasEntry())
		str << (IsPet() ? " Pet number: " : " Entry: ") << GetEntry() << " ";

	str << " Low: " << GetCounter();
	return str.str();
}

CWoWGuid CWoWGuid::Global(EWoWObjectHighGuid type, CounterType_t counter)
{
	return CWoWGuid(type, counter);
}

CWoWGuid CWoWGuid::MapSpecific(EWoWObjectHighGuid type, uint32 entry, CounterType_t counter)
{
	return CWoWGuid(type, entry, counter);
}


CByteBuffer& operator<<(CByteBuffer& buf, const CWoWGuid& guid)
{
	buf << uint64(guid.GetRawValue());
	return buf;
}

CByteBuffer& operator>>(CByteBuffer& buf, CWoWGuid& guid)
{
	uint64 value;
	buf.readBytes(&value, sizeof(uint64));
	guid.Set(value);
	return buf;
}