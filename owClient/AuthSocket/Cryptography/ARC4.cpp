#include "stdafx.h"

// General
#include "ARC4.h"

ARC4::ARC4(uint32 len) : m_ctx(EVP_CIPHER_CTX_new())
{
    EVP_CIPHER_CTX_init(m_ctx);
    EVP_EncryptInit(m_ctx, EVP_rc4(), nullptr, nullptr);

    EVP_CIPHER_CTX_set_key_length(m_ctx, len);
}

ARC4::ARC4(uint8* seed, uint32 len) : m_ctx(EVP_CIPHER_CTX_new())
{
    EVP_CIPHER_CTX_init(m_ctx);
    EVP_EncryptInit(m_ctx, EVP_rc4(), nullptr, nullptr);
    EVP_CIPHER_CTX_set_key_length(m_ctx, len);
    EVP_EncryptInit_ex(m_ctx, nullptr, nullptr, seed, nullptr);
}

ARC4::~ARC4()
{
    EVP_CIPHER_CTX_free(m_ctx);
}

void ARC4::Init(uint8* seed)
{
    EVP_EncryptInit(m_ctx, nullptr, seed, nullptr);
}

void ARC4::UpdateData(int len, uint8* data)
{
    int outlen = 0;
    EVP_EncryptUpdate(m_ctx, data, &outlen, data, len);
    EVP_EncryptFinal(m_ctx, data, &outlen);
}
