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

	void    ReadPackedGuid(CowGuid * WoWGuid);

	// IByteBuffer
	bool readBytes(void* Destination, size_t Size) override;
	void writeBytes(const void * Source, size_t BytesCount) override;

	void writeBytesInternal(const void * Source, size_t BytesCount);

protected:
	void ReadPackedGuid(uint8 mask, uint64 * value);

private:
    uint16  m_Size;
    Opcodes m_Opcode;
};