#include "stdafx.h"

// Genreal
#include "ServerPacket.h"

CServerPacket::CServerPacket(uint16 Size, Opcodes Opcode)
    : m_Size(Size)
    , m_Opcode(Opcode)
{
}

CServerPacket::~CServerPacket()
{
}



//
// CServerPacket
//
uint16 CServerPacket::GetPacketSize() const
{
    return m_Size;
}

Opcodes CServerPacket::GetPacketOpcode() const
{
    return m_Opcode;
}

bool CServerPacket::IsComplete() const
{
    return getSize() == static_cast<size_t>(GetPacketSize());
}

void CServerPacket::ReadPackedUInt64(uint64& guid)
{
	guid = 0;
	uint8_t mask;

	CByteBuffer::read(&mask);
	ReadPackedUInt64(mask, guid);
}

void CServerPacket::ReadPackedUInt64(uint8 mask, uint64& value)
{
	for (uint32 i = 0; i < 8; ++i)
	{
		if (mask & (uint8(1) << i))
		{
			uint8 byte;
			CByteBuffer::read(&byte);
			value |= (uint64(byte) << (i * 8));
		}
	}
}

void CServerPacket::AppendPackedUInt64(uint64 guid)
{
	uint8 mask = 0;
	size_t pos = CByteBuffer::getPos();
	*this << uint8(mask);

	uint8 packed[8];
	if (size_t packedSize = PackUInt64(guid, &mask, packed))
		CByteBuffer::writeBytes(packed, packedSize);

	getDataEx()[pos] = mask;
}

size_t CServerPacket::PackUInt64(uint64 value, uint8* mask, uint8* result)
{
	size_t resultSize = 0;
	*mask = 0;
	memset(result, 0, 8);

	for (uint8 i = 0; value != 0; ++i)
	{
		if (value & 0xFF)
		{
			*mask |= uint8(1 << i);
			result[resultSize++] = uint8(value & 0xFF);
		}

		value >>= 8;
	}

	return resultSize;
}

