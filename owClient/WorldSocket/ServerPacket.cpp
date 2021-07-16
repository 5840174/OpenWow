#include "stdafx.h"

// Genreal
#include "ServerPacket.h"

namespace
{

}

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

void CServerPacket::ReadPackedGuid(CowGuid * WoWGuid)
{
	if (WoWGuid == nullptr)
		throw CException("CServerPacket::ReadPackedGuid: WoWGuid is nullptr.");

	uint8 mask = 0;
	if (false == read(&mask))
		throw CException("CServerPacket::ReadPackedGuid: Unable to read packed guid mask.");

	uint64 wowPackedGuidAsUint64 = 0;
	ReadPackedGuid(mask, &wowPackedGuidAsUint64);

	*WoWGuid = CowGuid(wowPackedGuidAsUint64);
}



//
// IByteBuffer
//
bool CServerPacket::readBytes(void * Destination, size_t Size)
{
	if (false == __super::readBytes(Destination, Size))
		throw CException("CServerPacket: readBytes error. ByteBuffer ended. Opcode: '%d'.", m_Opcode);
	return true;
}

void CServerPacket::writeBytes(const void * /*Source*/, size_t /*BytesCount*/)
{
	throw CException("CServerPacket: writeBytes is not allowed.");
}

void CServerPacket::writeBytesInternal(const void * Source, size_t BytesCount)
{
	__super::writeBytes(Source, BytesCount);
}



//
// Protected
//
void CServerPacket::ReadPackedGuid(uint8 mask, uint64 * value)
{
	for (uint32 i = 0; i < 8; ++i)
	{
		if (mask & (uint8(1) << i))
		{
			uint8 byte;
			CByteBuffer::read(&byte);
			(*value) |= (uint64(byte) << (i * 8));
		}
	}
}