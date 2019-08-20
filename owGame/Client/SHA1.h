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

#ifndef _AUTH_SHA1_H
#define _AUTH_SHA1_H

#include <openssl/sha.h>

class BigNumber;

class SHA1Hash
{
public:
	SHA1Hash();
	virtual ~SHA1Hash();

	void UpdateBigNumbers(BigNumber* bn0, ...);

	void UpdateData(const uint8 *dta, int len);
	void UpdateData(const std::string &str);

	void Initialize();
	void Finalize();

	const uint8* GetDigest(void) const { return mDigest; }
	int GetLength(void) const { return SHA_DIGEST_LENGTH; }

    bool operator==(const SHA1Hash& Other);
    bool operator!=(const SHA1Hash& Other);

    bool operator==(const uint8* Bytes);
    bool operator!=(const uint8* Bytes);

	std::string toString();

private:
	SHA_CTX mC;
	uint8 mDigest[SHA_DIGEST_LENGTH];
};

/// Returns the SHA1 hash of the given content as hex std::string.
std::string CalculateSHA1Hash(std::string const& content);

#endif
