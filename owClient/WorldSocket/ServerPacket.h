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

	uint32  ReadUInt32();
	float   ReadFloat();
	double  ReadDouble();
	void    ReadPackedUInt64(uint64& guid);

	// IByteBuffer
	bool readBytes(void* Destination, size_t Size) override;
	//void writeBytes(const void * Source, size_t BytesCount) override;

protected:
	void ReadPackedUInt64(uint8 mask, uint64& value);

private:
    uint16  m_Size;
    Opcodes m_Opcode;
};