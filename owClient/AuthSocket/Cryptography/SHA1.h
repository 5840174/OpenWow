#pragma once

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
