#pragma once

#include "Opcodes.h"

class ZN_API CClientPacket 
	: public CByteBuffer
{
public:
    CClientPacket(Opcodes Opcode);
    virtual ~CClientPacket();

	// IByteBuffer
	bool readBytes(void* Destination, size_t Size) override;
	void writeBytes(const void * Source, size_t BytesCount) override;

    // CClientPacket
    void Complete();

private:
    Opcodes m_Opcode;      // to uint32
    bool    m_IsCompleted;
};