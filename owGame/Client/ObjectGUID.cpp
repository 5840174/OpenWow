#include "stdafx.h"

// General
#include "ObjectGUID.h"

// Additional
#include <sstream>
#include <iomanip>

ObjectGuid const ObjectGuid::Empty = ObjectGuid();

char const* ObjectGuid::GetTypeName(HighGuid high)
{
	switch (high)
	{
	case HighGuid::Item:         return "Item";
	case HighGuid::Player:       return "Player";
	case HighGuid::GameObject:   return "Gameobject";
	case HighGuid::Transport:    return "Transport";
	case HighGuid::Unit:         return "Creature";
	case HighGuid::Pet:          return "Pet";
	case HighGuid::Vehicle:      return "Vehicle";
	case HighGuid::DynamicObject: return "DynObject";
	case HighGuid::Corpse:       return "Corpse";
	case HighGuid::Mo_Transport: return "MoTransport";
	case HighGuid::Instance:     return "InstanceID";
	case HighGuid::Group:        return "Group";
	default:
		return "<unknown>";
	}
}

std::string ObjectGuid::ToString() const
{
	std::ostringstream str;
	str << "GUID Full: 0x" << std::hex << std::setw(16) << std::setfill('0') << _guid << std::dec;
	str << " Type: " << GetTypeName();
	if (HasEntry())
		str << (IsPet() ? " Pet number: " : " Entry: ") << GetEntry() << " ";

	str << " Low: " << GetCounter();
	return str.str();
}

ObjectGuid ObjectGuid::Global(HighGuid type, LowType counter)
{
	return ObjectGuid(type, counter);
}

ObjectGuid ObjectGuid::MapSpecific(HighGuid type, uint32 entry, LowType counter)
{
	return ObjectGuid(type, entry, counter);
}


CByteBuffer& operator<<(CByteBuffer& buf, const ObjectGuid& guid)
{
	buf << uint64(guid.GetRawValue());
	return buf;
}

CByteBuffer& operator>>(CByteBuffer& buf, ObjectGuid& guid)
{
	uint64 value;
	buf.readBytes(&value, sizeof(uint64));
	guid.Set(value);
	return buf;
}