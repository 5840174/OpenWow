#pragma once


class UpdateMask
{
public:
	/// Type representing how client reads update mask
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

	inline void SetBit(uint32 index)
	{
		((uint8 *)mUpdateMask)[index >> 3] |= 1 << (index & 0x7);
	}

	inline void UnsetBit(uint32 index)
	{
		((uint8 *)mUpdateMask)[index >> 3] &= (0xff ^ (1 << (index & 0x7)));
	}

	inline bool GetBit(uint32 index) const
	{
		return (((uint8 *)mUpdateMask)[index >> 3] & (1 << (index & 0x7))) != 0;
	}

	inline uint32 GetBlockCount() const { return mBlocks; }
	inline uint32 GetMaskLengthBytes() const { return mBlocks * (32 / 8); }
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


	inline void TEMP_InitMask(uint32 blocksCount)
	{
		mBlocks = blocksCount;

		mUpdateMask = new uint8[mBlocks * CLIENT_UPDATE_MASK_BITS];
		memset(mUpdateMask, 0, sizeof(uint8) * mBlocks * CLIENT_UPDATE_MASK_BITS);
	}

	inline void Clear()
	{
		if (mUpdateMask)
			memset(mUpdateMask, 0, mBlocks << 2);
	}

	inline UpdateMask& operator = (const UpdateMask& mask)
	{
		SetCount(mask.mCount);
		memcpy(mUpdateMask, mask.mUpdateMask, mBlocks << 2);

		return *this;
	}

	inline void operator &= (const UpdateMask& mask)
	{
		_ASSERT(mask.mCount <= mCount);
		for (uint32 i = 0; i < mBlocks; i++)
			mUpdateMask[i] &= mask.mUpdateMask[i];
	}

	inline void operator |= (const UpdateMask& mask)
	{
		_ASSERT(mask.mCount <= mCount);
		for (uint32 i = 0; i < mBlocks; i++)
			mUpdateMask[i] |= mask.mUpdateMask[i];
	}

	inline UpdateMask operator & (const UpdateMask& mask) const
	{
		_ASSERT(mask.mCount <= mCount);

		UpdateMask newmask;
		newmask = *this;
		newmask &= mask;

		return newmask;
	}

	inline UpdateMask operator | (const UpdateMask& mask) const
	{
		_ASSERT(mask.mCount <= mCount);

		UpdateMask newmask;
		newmask = *this;
		newmask |= mask;

		return newmask;
	}

private:
	uint32 mCount;
	uint32 mBlocks;
	uint8 *mUpdateMask;
};

/*
class UpdateMask
{
public:
	/// Type representing how client reads update mask
	typedef uint32 ClientUpdateMaskType;

	enum UpdateMaskCount
	{
		CLIENT_UPDATE_MASK_BITS = sizeof(ClientUpdateMaskType) * 8,
	};

	UpdateMask() : _fieldCount(0), _blockCount(0), _bits(nullptr) {}

	UpdateMask(UpdateMask const& right) : _bits(nullptr)
	{
		SetCount(right.GetCount());
		memcpy(_bits, right._bits, sizeof(uint8) * _blockCount * 32);
	}

	~UpdateMask() { delete[] _bits; }

	void SetBit(uint32 index) { _bits[index] = 1; }
	void UnsetBit(uint32 index) { _bits[index] = 0; }
	bool GetBit(uint32 index) const { return _bits[index] != 0; }
	uint8* GetBits() { return _bits; }


	uint32 GetBlockCount() const { return _blockCount; }
	uint32 GetCount() const { return _fieldCount; }

	void SetCount(uint32 valuesCount)
	{
		delete[] _bits;

		_fieldCount = valuesCount;
		_blockCount = (valuesCount + CLIENT_UPDATE_MASK_BITS - 1) / CLIENT_UPDATE_MASK_BITS;

		_bits = new uint8[_blockCount * CLIENT_UPDATE_MASK_BITS];
		memset(_bits, 0, sizeof(uint8) * _blockCount * CLIENT_UPDATE_MASK_BITS);
	}

	void Clear()
	{
		if (_bits)
			memset(_bits, 0, sizeof(uint8) * _blockCount * CLIENT_UPDATE_MASK_BITS);
	}

	UpdateMask& operator=(UpdateMask const& right)
	{
		if (this == &right)
			return *this;

		SetCount(right.GetCount());
		memcpy(_bits, right._bits, sizeof(uint8) * _blockCount * CLIENT_UPDATE_MASK_BITS);
		return *this;
	}

	UpdateMask& operator&=(UpdateMask const& right)
	{
		_ASSERT(right.GetCount() <= GetCount());
		for (uint32 i = 0; i < _fieldCount; ++i)
			_bits[i] &= right._bits[i];

		return *this;
	}

	UpdateMask& operator|=(UpdateMask const& right)
	{
		_ASSERT(right.GetCount() <= GetCount());
		for (uint32 i = 0; i < _fieldCount; ++i)
			_bits[i] |= right._bits[i];

		return *this;
	}

	UpdateMask operator|(UpdateMask const& right)
	{
		UpdateMask ret(*this);
		ret |= right;
		return ret;
	}

private:
	uint32 _fieldCount;
	uint32 _blockCount;
	uint8* _bits;
};*/
