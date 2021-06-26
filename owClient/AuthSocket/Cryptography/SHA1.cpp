/*
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

// General
#include "SHA1.h"

// Additional
#include "BigNumber.h"

SHA1Hash::SHA1Hash()
{
    SHA1_Init(&mC);
    memset(mDigest, 0, SHA_DIGEST_LENGTH * sizeof(uint8));
}

SHA1Hash::~SHA1Hash()
{
    SHA1_Init(&mC);
}

void SHA1Hash::UpdateData(const uint8 *dta, int len)
{
    SHA1_Update(&mC, dta, len);
}

void SHA1Hash::UpdateData(const std::string &str)
{
    UpdateData((uint8 const*)str.c_str(), str.length());
}

void SHA1Hash::UpdateBigNumbers(BigNumber* bn0, ...)
{
    va_list v;
    BigNumber* bn;

    va_start(v, bn0);
    bn = bn0;
    while (bn)
    {
        UpdateData(bn->AsByteArray().get(), bn->GetNumBytes());
        bn = va_arg(v, BigNumber*);
    }
    va_end(v);
}

void SHA1Hash::Initialize()
{
    SHA1_Init(&mC);
}

void SHA1Hash::Finalize(void)
{
    SHA1_Final(mDigest, &mC);
}

bool SHA1Hash::operator==(const SHA1Hash& Other)
{
    for (uint32 i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        if (Other.GetDigest()[i] != GetDigest()[i])
        {
            return false;
        }
    }

    return true;
}

bool SHA1Hash::operator!=(const SHA1Hash & Other)
{
    return !((*this) == Other);
}

bool SHA1Hash::operator==(const uint8 * Bytes)
{
    for (uint32 i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        if (Bytes[i] != GetDigest()[i])
        {
            return false;
        }
    }

    return true;
}

bool SHA1Hash::operator!=(const uint8 * Bytes)
{
    return !((*this) == Bytes);
}

std::string SHA1Hash::toString()
{
	std::string result = "";

	for (uint32 i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		char buf[4];
		sprintf_s(buf, "%02X", mDigest[i]);
		result += buf;
	}

	return result;
}

// TODO: Move to utils
std::string ByteArrayToHexStr(uint8 const* bytes, uint32 arrayLen, bool reverse  = false )
{
	int32 init = 0;
	int32 end = arrayLen;
	int8 op = 1;

	if (reverse)
	{
		init = arrayLen - 1;
		end = -1;
		op = -1;
	}

	std::ostringstream ss;
	for (int32 i = init; i != end; i += op)
	{
		char buffer[4];
		sprintf_s(buffer, "%02X", bytes[i]);
		ss << buffer;
	}

	return ss.str();
}

std::string CalculateSHA1Hash(std::string const& content)
{
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)content.c_str(), content.length(), (unsigned char*)&digest);

    return ByteArrayToHexStr(digest, SHA_DIGEST_LENGTH);
}
