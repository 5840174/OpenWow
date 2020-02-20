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

	// PackedGUID
	void ReadPackedUInt64(uint64& guid);
	void ReadPackedUInt64(uint8 mask, uint64& value);
	void AppendPackedUInt64(uint64 guid);
	size_t PackUInt64(uint64 value, uint8* mask, uint8* result);

private:
    uint16  m_Size;
    Opcodes m_Opcode;
};