#pragma once

#include "Opcodes.h"

class ZN_API CClientPacket 
	: public CByteBuffer
{
public:
    CClientPacket(Opcodes Opcode);
    virtual ~CClientPacket();

	// CClientPacket
	void Complete();
	void AppendPackedUInt64(uint64 guid);

	// IByteBuffer
	bool readBytes(void* Destination, size_t Size) override;
	void writeBytes(const void * Source, size_t BytesCount) override;

protected:
	size_t PackUInt64(uint64 value, uint8* mask, uint8* result);

private:
    Opcodes m_Opcode;      // to uint32
    bool    m_IsCompleted;
};