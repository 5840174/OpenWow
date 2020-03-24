#pragma once

#include "Opcodes.h"

class ZN_API CServerPacket 
	: public CByteBuffer
{
public:
    CServerPacket(uint16 Size, Opcodes Opcode);
    virtual ~CServerPacket();

    // CServerPacket
    uint16  GetPacketSize() const;
    Opcodes GetPacketOpcode() const;
    bool    IsComplete() const;

private:
    uint16  m_Size;
    Opcodes m_Opcode;
};