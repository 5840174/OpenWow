/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2009-2010 TrinityZero <http://www.trinityzero.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "stdafx.h"

 // General
#include "AuthCrypt.h"

 // Additional
#include "HMACSHA1.h"

#if WOW_CLIENT_VERSION < WOW_WOTLK_3_3_5

AuthCrypt::AuthCrypt()
{
    _initialized = false;
}

void AuthCrypt::Init()
{
    _send_i = _send_j = _recv_i = _recv_j = 0;
    _initialized = true;
}

void AuthCrypt::DecryptRecv(uint8 *data, size_t len)
{
    if (!_initialized)
    {
        return;
    }

    if (len > CRYPTED_RECV_LEN)
    {
        _ASSERT(false);
        return;
    }

    for (size_t t = 0; t < len; t++)
    {
        _recv_i %= _key.size();
        uint8 x = (data[t] - _recv_j) ^ _key[_recv_i];
        ++_recv_i;
        _recv_j = data[t];
        data[t] = x;
    }
}

void AuthCrypt::EncryptSend(uint8 *data, size_t len)
{
    if (!_initialized)
    {
        return;
    }

    //if (len > CRYPTED_SEND_LEN)
    //{
    //    _ASSERT(false);
    //    return;
    //}

    for (size_t t = 0; t < len; t++)
    {
        _send_i %= _key.size();
        uint8 x = (data[t] ^ _key[_send_i]) + _send_j;
        ++_send_i;
        data[t] = _send_j = x;
    }
}

void AuthCrypt::SetKey(uint8 *key, size_t len)
{
    _key.resize(len);
    std::copy(key, key + len, _key.begin());
}


AuthCrypt::~AuthCrypt()
{
}

void AuthCrypt::GenerateKey(uint8 *key, BigNumber *bn)
{
    HmacHash hash(bn->GetNumBytes(), bn->AsByteArray().get());
    memcpy(key, hash.GetDigest(), SHA_DIGEST_LENGTH);
}

#else

AuthCrypt::AuthCrypt() :
	_clientDecrypt(SHA_DIGEST_LENGTH), _serverEncrypt(SHA_DIGEST_LENGTH),
	_initialized(false)
{ }

void AuthCrypt::Init(BigNumber* K)
{
	uint8 ServerEncryptionKey[SEED_KEY_SIZE] = { 0xCC, 0x98, 0xAE, 0x04, 0xE8, 0x97, 0xEA, 0xCA, 0x12, 0xDD, 0xC0, 0x93, 0x42, 0x91, 0x53, 0x57 };
	HmacHash serverEncryptHmac(SEED_KEY_SIZE, (uint8*)ServerEncryptionKey);
	uint8 *encryptHash = serverEncryptHmac.ComputeHash(K);

	uint8 ServerDecryptionKey[SEED_KEY_SIZE] = { 0xC2, 0xB3, 0x72, 0x3C, 0xC6, 0xAE, 0xD9, 0xB5, 0x34, 0x3C, 0x53, 0xEE, 0x2F, 0x43, 0x67, 0xCE };
	HmacHash clientDecryptHmac(SEED_KEY_SIZE, (uint8*)ServerDecryptionKey);
	uint8 *decryptHash = clientDecryptHmac.ComputeHash(K);

	//ARC4 _serverDecrypt(encryptHash);
	_clientDecrypt.Init(decryptHash);
	_serverEncrypt.Init(encryptHash);
	//ARC4 _clientEncrypt(decryptHash);

	// Drop first 1024 bytes, as WoW uses ARC4-drop1024.
	uint8 syncBuf[1024];
	memset(syncBuf, 0, 1024);

	_serverEncrypt.UpdateData(1024, syncBuf);
	//_clientEncrypt.UpdateData(1024, syncBuf);

	memset(syncBuf, 0, 1024);

	//_serverDecrypt.UpdateData(1024, syncBuf);
	_clientDecrypt.UpdateData(1024, syncBuf);

	_initialized = true;
}

void AuthCrypt::DecryptRecv(uint8 *data, size_t len)
{
	if (!_initialized)
		return;

	_serverEncrypt.UpdateData(len, data);

	
}

void AuthCrypt::EncryptSend(uint8 *data, size_t len)
{
	if (!_initialized)
		return;

	_clientDecrypt.UpdateData(len, data);
}

#endif

