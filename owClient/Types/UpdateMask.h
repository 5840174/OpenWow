#pragma once

class UpdateMask
{
public:
	typedef uint32 ClientUpdateMaskType;

	enum UpdateMaskCount
	{
		CLIENT_UPDATE_MASK_BITS = sizeof(ClientUpdateMaskType) * 8,
	};

	UpdateMask() 
		: mCount(0)
		, mBlocks(0)
		, mUpdateMask(0) 
	{}

	UpdateMask(const UpdateMask& mask) 
		: mUpdateMask(0) 
	{ 
		*this = mask; 
	}

	~UpdateMask()
	{
		if (mUpdateMask)
			delete[] mUpdateMask;
	}

	inline bool GetBit(uint32 index) const
	{
		return (((uint8 *)mUpdateMask)[index >> 3] & (1 << (index & 0x7))) != 0;
	}

	inline uint32 GetBlockCount() const { return mBlocks; }
	inline uint32 GetMaskLengthBytes() const { return mBlocks * 4; }
	inline uint32 GetCount() const { return mCount; }
	inline uint8* GetMask() { return (uint8*)mUpdateMask; }

	inline void SetCount(uint32 valuesCount)
	{
		if (mUpdateMask)
			delete[] mUpdateMask;

		mCount = valuesCount;
		mBlocks = (valuesCount + CLIENT_UPDATE_MASK_BITS - 1) / CLIENT_UPDATE_MASK_BITS;

		mUpdateMask = new uint8[mBlocks * CLIENT_UPDATE_MASK_BITS];
		memset(mUpdateMask, 0, sizeof(uint8) * mBlocks * CLIENT_UPDATE_MASK_BITS);
	}

	inline void Clear()
	{
		if (mUpdateMask)
			memset(mUpdateMask, 0, mBlocks << 2);
	}

	inline UpdateMask& operator= (const UpdateMask& mask)
	{
		SetCount(mask.mCount);
		memcpy(mUpdateMask, mask.mUpdateMask, mBlocks << 2);

		return *this;
	}

private:
	uint32 mCount;
	uint32 mBlocks;
	uint8 *mUpdateMask;
};
