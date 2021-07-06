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

uint32 CServerPacket::ReadUInt32()
{
	uint32 value = 0;
	readBytes(&value, sizeof(uint32));
	return value;
}

float CServerPacket::ReadFloat()
{
	float value = 0.0f;
	readBytes(&value, sizeof(float));
	return value;
}

double CServerPacket::ReadDouble()
{
	double value = 0.0;
	readBytes(&value, sizeof(double));
	return value;
}

void CServerPacket::ReadPackedUInt64(uint64& guid)
{
	guid = 0;
	uint8_t mask;
	read(&mask);

	ReadPackedUInt64(mask, guid);
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

//void CServerPacket::writeBytes(const void * /*Source*/, size_t /*BytesCount*/)
//{
//	throw CException("CServerPacket: writeBytes is not allowed.");
//}



//
// Protected
//
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