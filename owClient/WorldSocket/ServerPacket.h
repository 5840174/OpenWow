#pragma once

#include "Opcodes.h"

class ZN_API CServerPacket 
	: public CByteBuffer
{
public:
    CServerPacket(uint16 Size, Opcodes Opcode);
    virtual ~CServerPacket();

	// IByteBuffer
	bool readBytes(void* Destination, size_t Size) override;
	//void writeBytes(const void * Source, size_t BytesCount) override;

    // CServerPacket
    uint16  GetPacketSize() const;
    Opcodes GetPacketOpcode() const;
    bool    IsComplete() const;

private:
    uint16  m_Size;
    Opcodes m_Opcode;
};