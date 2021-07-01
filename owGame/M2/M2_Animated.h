#pragma once

#include "M2_AnimatedConverters.h"
#include "M2_Types.h"

/*
	Generic animated value class:

	T is the values type to animate
	D is the values type stored in the file (by default this is the same as T)
	Conv is a conversion object that defines T conv(D) to convert from D to T (by default this is an identity function)	(there might be a nicer way to do this? meh meh)
*/
#if WOW_CLIENT_VERSION < WOW_WOTLK_3_3_5
template <class T, class D = T, class Conv = NoConvert<T> >
class M2_Animated
{
public:
	M2_Animated()
		: m_Type(Interpolations::None)
		, m_GlobalSecIndex(-1)
	{}

	inline void Initialize(const M2Track<D>& b, const std::shared_ptr<IFile>& File, const std::vector<std::shared_ptr<IFile>>& /*AnimFiles*/, T fixfunc(const T&) = NoFix)
	{
		m_Type = b.interpolation_type;
		m_GlobalSecIndex = b.global_sequence;

		_ASSERT((b.interpolation_ranges.size > 0) || (m_GlobalSecIndex != -1) || (m_Type == Interpolations::None));

		// ranges
		if (b.interpolation_ranges.size > 0)
		{
			const uint32* ranges = (const uint32*)(File->getData() + b.interpolation_ranges.offset);
			for (uint32 i = 0; i < b.interpolation_ranges.size; i += 2)
			{
				m_Ranges.push_back(std::make_pair(ranges[i], ranges[i + 1]));
			}
		}

		// times
		const uint32* times = (const uint32*)(File->getData() + b.timestamps.offset);
		for (uint32 i = 0; i < b.timestamps.size; i++)
			m_Times.push_back(times[i]);

		_ASSERT(b.timestamps.size == b.values.size);

		// keyframes
		const D* values = (const D*)(File->getData() + b.values.offset);
		for (uint32 i = 0; i < b.values.size; i++)
		{
			switch (m_Type)
			{
				case Interpolations::None:
				case Interpolations::Linear:
					m_Values.push_back(fixfunc(Conv::conv(values[i])));
					break;

				case Interpolations::Hermite:
					m_Values.push_back(fixfunc(Conv::conv(values[i * 3 + 0])));
					m_ValuesHermiteIn.push_back(fixfunc(Conv::conv(values[i * 3 + 1])));
					m_ValuesHermiteOut.push_back(fixfunc(Conv::conv(values[i * 3 + 2])));
					break;

				//default:
				//	_ASSERT_EXPR(false, "M2_Animated: Unknown interpolation type.");
			}
		}

	}

	inline bool IsUsesBySequence(uint16 SequenceIndex) const
	{
		if (m_Type == Interpolations::None)
			return m_Values.size() == 1;

		if (m_GlobalSecIndex == -1 && m_Ranges.size() <= SequenceIndex)
			return false;

		if (m_Values.empty())
			return false; // ????

		return true;
	}

	inline T GetValue(uint16 SequenceIndex, uint32 time, const std::vector<SM2_Loop>& GlobalLoop, const uint32 GlobalTime) const
	{
		_ASSERT(m_Values.empty() == false);
		if (m_Values.size() == 1)
			return m_Values[0];

        std::pair<uint32, uint32> range = std::make_pair(0, m_Values.size() - 1);

		// obtain a time value and a values range
		if (m_GlobalSecIndex != -1)
		{
			time = 0;

			_ASSERT(m_GlobalSecIndex >= 0 && m_GlobalSecIndex < GlobalLoop.size());
			uint32 globalLoopTimeStamp = GlobalLoop[m_GlobalSecIndex].timestamp;
			if (globalLoopTimeStamp != 0)
				time = GlobalTime % globalLoopTimeStamp;
		}
		else
		{
			range = m_Ranges[SequenceIndex];

			// Bug?
			if (time < m_Times[0] || time >= m_Times[m_Times.size() - 1])
				return m_Values[range.first];
		}

		// If simple frame
		if (range.first == range.second)
			return m_Values[range.first];

		return GetInterpolatedValue(range, time);
	}

private:
	inline uint32 GetTimesIndex(const std::pair<uint32, uint32>& Range, uint32 Time) const
	{
		for (uint32 i = Range.first; i < Range.second; i++)
			if (Time >= m_Times[i] && Time < m_Times[i + 1])
				return i;

		_ASSERT_EXPR(false, "Unexpected behaviour");
		return UINT32_MAX;
	}

	inline T GetInterpolatedValue(const std::pair<uint32, uint32>& Range, uint32 Time) const
	{
		uint32 timeIndex = GetTimesIndex(Range, Time);

		uint32 t1 = m_Times[timeIndex];
		uint32 t2 = m_Times[timeIndex + 1];
		_ASSERT((t2 > t1) && (Time >= t1) && (Time < t2));

		float r = static_cast<float>(Time - t1) / static_cast<float>(t2 - t1);

		switch (m_Type)
		{
			case Interpolations::None:
				return interpolateNone<T>(r, m_Values[timeIndex], m_Values[timeIndex + 1]);

			case Interpolations::Linear:
				return interpolateLinear<T>(r, m_Values[timeIndex], m_Values[timeIndex + 1]);

			case Interpolations::Hermite:
				return interpolateHermite<T>(r, m_Values[timeIndex], m_Values[timeIndex + 1], m_ValuesHermiteIn[timeIndex], m_ValuesHermiteOut[timeIndex]);

			default:
				_ASSERT_EXPR(false, "M2_Animated: Unknown interpolation type.");
		}

		return m_Values[0];
	}

private:
	Interpolations m_Type;
	int16 m_GlobalSecIndex;

	std::vector<std::pair<uint32, uint32>>m_Ranges;
    std::vector<uint32> m_Times;

    std::vector<T> m_Values;
    std::vector<T> m_ValuesHermiteIn;
    std::vector<T> m_ValuesHermiteOut;
};
#else
template <class T, class D = T, class Conv = NoConvert<T> >
class M2_Animated
{
public:
	M2_Animated()
		: m_Type(Interpolations::None)
		, m_GlobalSecIndex(-1)
	{}

	inline void Initialize(const M2Track<D>& b, const std::shared_ptr<IFile>& File, const std::vector<std::shared_ptr<IFile>>& AnimFiles, T fixfunc(const T&) = NoFix)
	{
		m_Type = b.interpolation_type;
		m_GlobalSecIndex = b.global_sequence;

		_ASSERT(b.timestamps.size == b.values.size);

		// Prepare data
		m_Times.resize(b.timestamps.size);
		m_Values.resize(b.values.size);
		m_ValuesHermiteIn.resize(b.values.size);
		m_ValuesHermiteOut.resize(b.values.size);

		// times
		M2Array<uint32>* pHeadTimes = (M2Array<uint32>*)(File->getData() + b.timestamps.offset);
		M2Array<D>* pHeadValues = (M2Array<D>*)(File->getData() + b.values.offset);
		for (uint32 j = 0; j < GetCount(); j++)
		{
			uint32* times = nullptr;
			D* values = nullptr;
			if (AnimFiles.at(j) != nullptr)
			{
				_ASSERT(pHeadTimes[j].offset < AnimFiles.at(j)->getSize());
				times = (uint32*)(AnimFiles.at(j)->getData() + pHeadTimes[j].offset);
				_ASSERT(pHeadValues[j].offset < AnimFiles.at(j)->getSize());
				values = (D*)(AnimFiles.at(j)->getData() + pHeadValues[j].offset);
			}
			else
			{
				_ASSERT(pHeadTimes[j].offset < File->getSize());
				times = (uint32*)(File->getData() + pHeadTimes[j].offset);
				_ASSERT(pHeadValues[j].offset < File->getSize());
				values = (D*)(File->getData() + pHeadValues[j].offset);
			}

			_ASSERT(times != nullptr);
			m_Times[j].assign(times, times + pHeadTimes[j].size);
			//for (uint32 i = 0; i < pHeadTimes[j].size; i++)
			//	m_Times[j].push_back(times[i]);

			_ASSERT(values != nullptr);
			for (uint32 i = 0; i < pHeadValues[j].size; i++)
			{
				switch (m_Type)
				{
				case Interpolations::None:
				case Interpolations::Linear:
					m_Values[j].push_back(fixfunc(Conv::conv(values[i])));
					break;

				case Interpolations::Hermite:
					m_Values[j].push_back(fixfunc(Conv::conv(values[i * 3 + 0])));
					m_ValuesHermiteIn[j].push_back(fixfunc(Conv::conv(values[i * 3 + 1])));
					m_ValuesHermiteOut[j].push_back(fixfunc(Conv::conv(values[i * 3 + 2])));
					break;

					//default:
					//	_ASSERT_EXPR(false, "M2_Animated: Unknown interpolation type.");
				}
			}
		}
	}

	inline bool IsUsesBySequence(uint16 SequenceIndex) const
	{
		if (IsStaticValue())
			return true;

		// Global sec always use
		if (m_GlobalSecIndex != -1)
			return true;

		if (SequenceIndex < GetCount())
		{
			if (m_Values[SequenceIndex].empty())
				return false;

			return true;
		}

		return false;
	}

	inline T GetValue(uint16 SequenceIndex, uint32 time, const std::vector<SM2_Loop>& GlobalLoop, const uint32 GlobalTime) const
	{
		if (IsStaticValue())
		{
			return m_Values.at(0).at(0);
		}

		// obtain a time value and a values range
		if (m_GlobalSecIndex != -1)
		{
			time = 0;
			SequenceIndex = 0;

			_ASSERT(m_GlobalSecIndex >= 0 && m_GlobalSecIndex < GlobalLoop.size());
			uint32 globalLoopTimeStamp = GlobalLoop[m_GlobalSecIndex].timestamp;
			if (globalLoopTimeStamp != 0)
				time = GlobalTime % globalLoopTimeStamp;
		}

		_ASSERT(SequenceIndex < GetCount());

		const std::vector<uint32>& pTimes = m_Times[SequenceIndex];
		const std::vector<T>& pData = m_Values[SequenceIndex];

		if (SequenceIndex < GetCount() && pData.size() > 1 && pTimes.size() > 1)
		{
			int max_time = pTimes.at(pTimes.size() - 1);
			if (max_time > 0)
				time %= max_time; // I think this might not be necessary?

			uint32 pos = 0;
			for (uint32 i = 0; i < pTimes.size() - 1; i++)
			{
				if (time >= pTimes.at(i) && time < pTimes.at(i + 1))
				{
					pos = i;
					break;
				}
			}

			uint32 t1 = pTimes.at(pos);
			uint32 t2 = pTimes.at(pos + 1);
			float r = static_cast<float>(time - t1) / static_cast<float>(t2 - t1);
			switch (m_Type)
			{
				case Interpolations::None:
					return interpolateNone(r, pData.at(pos), pData.at(pos + 1));
				case Interpolations::Linear:
					return interpolateLinear<T>(r, pData.at(pos), pData.at(pos + 1));
				case Interpolations::Hermite:
					return interpolateHermite<T>(r, pData.at(pos), pData.at(pos + 1), m_ValuesHermiteIn[SequenceIndex].at(pos), m_ValuesHermiteOut[SequenceIndex].at(pos));
				default:
					_ASSERT_EXPR(false, "M2_Animated: Unknown interpolation type.");
			}
		}
		else if (! pData.empty())
		{
			return pData.at(0);
		}

		return T();
	}

private:
	inline size_t GetCount() const
	{
		return m_Times.size();
	}

	inline bool IsStaticValue() const
	{
		if (m_GlobalSecIndex == -1 && m_Type == Interpolations::None && GetCount() == 1)
		{
			//_ASSERT(m_Times.at(0).size() == 1);
			//_ASSERT(m_Values.at(0).size() == 1);
			return true;
		}

		return false;
	}

private:
	Interpolations                   m_Type;
	int16                            m_GlobalSecIndex;

	std::vector<std::vector<uint32>> m_Times;
	std::vector<std::vector<T>>      m_Values;
	std::vector<std::vector<T>>      m_ValuesHermiteIn;
	std::vector<std::vector<T>>      m_ValuesHermiteOut;
};

#endif
