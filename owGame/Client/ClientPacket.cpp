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
    _ASSERT(m_IsCompleted == false);

    uint16 size0             = getSize() + sizeof(uint32) /* Opcode size */;
    uint16 sizeConverted     = static_cast<uint16>((&reinterpret_cast<uint8&>(size0))[0] << 8 | (&reinterpret_cast<uint8&>(size0))[1]);
    uint32 opcodeConverted   = static_cast<uint32>((&reinterpret_cast<uint8&>(m_Opcode))[1] << 8 | (&reinterpret_cast<uint8&>(m_Opcode))[0]);

    Insert(0,              reinterpret_cast<const uint8*>(&sizeConverted),   sizeof(uint16));
    Insert(sizeof(uint16), reinterpret_cast<const uint8*>(&opcodeConverted), sizeof(uint32));

    m_IsCompleted = true;
    // Don't forget about crypto!!!
}
