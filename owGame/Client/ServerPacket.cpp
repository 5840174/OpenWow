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



