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



