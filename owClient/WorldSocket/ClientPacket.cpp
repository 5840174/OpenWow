#include "stdafx.h"

// General
#include "ClientPacket.h"

CClientPacket::CClientPacket(Opcodes Opcode)
    : m_Opcode(Opcode)
    , m_IsCompleted(false)
{
}

CClientPacket::~CClientPacket()
{
}



//
// CClientPacket
//
void CClientPacket::Complete()
{
	if (m_IsCompleted)
		throw CException("CClientPacket::Complete: Client packet already completed.");

	uint16 size0 = getSize() + sizeof(uint32) /* Opcode size */;
	uint16 sizeConverted = static_cast<uint16>((&reinterpret_cast<uint8&>(size0))[0] << 8 | (&reinterpret_cast<uint8&>(size0))[1]);
	uint32 opcodeConverted = static_cast<uint32>((&reinterpret_cast<uint8&>(m_Opcode))[1] << 8 | (&reinterpret_cast<uint8&>(m_Opcode))[0]);

	insertBytes(0,              reinterpret_cast<const uint8*>(&sizeConverted),   sizeof(uint16));
	insertBytes(sizeof(uint16), reinterpret_cast<const uint8*>(&opcodeConverted), sizeof(uint32));

	m_IsCompleted = true;
}

void CClientPacket::AppendPackedUInt64(uint64 guid)
{
	uint8 mask = 0;
	size_t pos = getPos();
	*this << uint8(mask);

	uint8 packed[8];
	if (size_t packedSize = PackUInt64(guid, &mask, packed))
		writeBytes(packed, packedSize);

	getDataEx()[pos] = mask;
}



//
// IByteBuffer
//
bool CClientPacket::readBytes(void * Destination, size_t Size)
{
	throw CException("CClientPacket: readBytes is not allowed.");
}

void CClientPacket::writeBytes(const void * Source, size_t BytesCount)
{
	__super::writeBytes(Source, BytesCount);
}



//
// Protected
//
size_t CClientPacket::PackUInt64(uint64 value, uint8* mask, uint8* result)
{
	size_t resultSize = 0;
	*mask = 0;
	std::memset(result, 0x00, 8);

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
