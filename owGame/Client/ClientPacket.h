#pragma once

#include "Opcodes.h"

class CClientPacket 
	: public CByteBuffer
{
public:
    CClientPacket(Opcodes Opcode);
    virtual ~CClientPacket();

    // CClientPacket
    void Complete();

private:
    Opcodes m_Opcode;      // to uint32
    bool    m_IsCompleted;
};