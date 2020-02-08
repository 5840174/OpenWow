#pragma once

struct AuthChallenge_C : public ISendable
{
	AuthChallenge_C(std::string Login, uint32 IPv4) 
        : Login(Login)
        , IPv4(IPv4)
	{}

	void Send(TcpSocket* _socket) override
	{
		CByteBuffer bb;
		bb << (uint8)AUTH_LOGON_CHALLENGE;
		bb << (uint8)6;
		bb << (uint8)(Login.size() + 30);
		bb << (uint8)0;

		bb.writeBytes(gamename, 4);
		bb << version1;
		bb << version2;
		bb << version3;
		bb << build;
		bb.writeBytes((const uint8*)"68x", 4);   // x86
		bb.writeBytes((const uint8*)"niW", 4);   // Win
		bb.writeBytes((const uint8*)"SUne", 4);  // ruRU
		bb << (uint32)180;
		bb << IPv4;
		bb << (uint8)Login.size();
		bb.writeString(Login);

        _socket->SendBuf(reinterpret_cast<const char*>(bb.getData()), bb.getSize());
	}

	//--

	uint8   gamename[4] = {};
    const uint8   version1 = 1;
    const uint8   version2 = 12;
    const uint8   version3 = 1;
	const uint16  build = 5875;
	uint32  IPv4;
	std::string	Login;
};