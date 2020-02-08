#pragma once

struct AuthProof_C : public ISendable
{
	AuthProof_C(uint8* _A, const uint8* _MClient)
	{
		memcpy(A, _A, 32);
		memcpy(M1, _MClient, SHA_DIGEST_LENGTH);
	}

	void Send(TcpSocket* _socket) override
	{
		CByteBuffer bb;
		bb << (uint8)AUTH_LOGON_PROOF;
		bb.writeBytes(A, 32);
		bb.writeBytes(M1, SHA_DIGEST_LENGTH);
        bb.writeDummy(20);
		bb << (uint8)0;
		bb << (uint8)0;

		_socket->SendBuf(reinterpret_cast<const char*>(bb.getData()), bb.getSize());
	}

	uint8   A[32];
	uint8   M1[SHA_DIGEST_LENGTH];
};