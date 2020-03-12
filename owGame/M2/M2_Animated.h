#pragma once

#include "M2_AnimatedConverters.h"
#include "M2_Types.h"

/*
	Generic animated value class:

	T is the values type to animate
	D is the values type stored in the file (by default this is the same as T)
	Conv is a conversion object that defines T conv(D) to convert from D to T (by default this is an identity function)	(there might be a nicer way to do this? meh meh)
*/
template <class T, class D = T, class Conv = NoConvert<T> >
class M2_Animated
{
public:
	M2_Animated()
		: m_Type(Interpolations::INTERPOLATION_NONE)
		, m_GlobalSecIndex(-1)
	{}

	inline void Initialize(const M2Track<D>& b, const std::shared_ptr<IFile>& File, T fixfunc(const T&) = NoFix)
	{
		m_Type = b.interpolation_type;
		m_GlobalSecIndex = b.global_sequence;

		_ASSERT((b.interpolation_ranges.size > 0) || (m_GlobalSecIndex != -1) || (m_Type == Interpolations::INTERPOLATION_NONE));

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
				case Interpolations::INTERPOLATION_NONE:
				case Interpolations::INTERPOLATION_LINEAR:
					m_Values.push_back(fixfunc(Conv::conv(values[i])));
					break;

				case Interpolations::INTERPOLATION_HERMITE:
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
		if (m_Type == Interpolations::INTERPOLATION_NONE)
			return m_Values.size() == 1;

		if (m_GlobalSecIndex == -1 && m_Ranges.size() <= SequenceIndex)
			return false;

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
			_ASSERT(time >= m_Times[0] && time < m_Times[m_Times.size() - 1]);
			range = m_Ranges[SequenceIndex];
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
			case Interpolations::INTERPOLATION_NONE:
				return m_Values[timeIndex];

			case Interpolations::INTERPOLATION_LINEAR:
				return interpolate<T>(r, m_Values[timeIndex], m_Values[timeIndex + 1]);

			case Interpolations::INTERPOLATION_HERMITE:
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
