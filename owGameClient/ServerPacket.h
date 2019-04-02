#pragma once

#include "Opcodes.h"

class CServerPacket : public CByteBuffer
{
public:
    CServerPacket(uint16 Size, Opcodes Opcode);
    virtual ~CServerPacket();

    // CServerPacket
    uint16  GetPacketSize() const;
    Opcodes GetPacketOpcode() const;

private:
    uint16  m_Size;
    Opcodes m_Opcode;
};