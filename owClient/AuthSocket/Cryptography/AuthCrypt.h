#pragma once

#include "BigNumber.h"
#include "ARC4.h"

#if WOW_CLIENT_VERSION < WOW_WOTLK_3_3_5
class AuthCrypt
{
    public:
        AuthCrypt();
        ~AuthCrypt();

        const static size_t CRYPTED_SEND_LEN = 4;
        const static size_t CRYPTED_RECV_LEN = 6;

        void Init();

        //void SetKey(BigNumber *);
        void SetKey(uint8 *key, size_t len);

        void DecryptRecv(uint8 *, size_t);
        void EncryptSend(uint8 *, size_t);

        bool IsInitialized() { return _initialized; }

        static void GenerateKey(uint8 *, BigNumber * K);

    private:
        std::vector<uint8> _key;
        uint8 _send_i, _send_j, _recv_i, _recv_j;
        bool _initialized;
};
#else

class AuthCrypt
{
public:
	AuthCrypt();

	void Init(BigNumber* K);
	void DecryptRecv(uint8 *, size_t);
	void EncryptSend(uint8 *, size_t);

	bool IsInitialized() const { return _initialized; }

private:
	ARC4 _clientDecrypt;
	ARC4 _serverEncrypt;
	bool _initialized;
};

#endif
