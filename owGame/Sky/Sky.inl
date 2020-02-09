#pragma once

template<typename T>
inline T GetByTimeTemplate(std::vector<Sky::SkyParam<T>>* _array, uint32 _paramNum, uint32 _time)
{
	const std::vector<Sky::SkyParam<T>>& param = _array[_paramNum];
	if (param.empty())
	{
		return T();
	}

	T parBegin, parEnd;
	uint32 timeBegin, timeEnd;
	uint32_t last = static_cast<uint32>(param.size()) - 1;

	if (_time < param[0].time)
	{
		// interpolate between last and first
		parBegin = param[last].value;
		timeBegin = param[last].time;
		parEnd = param[0].value;
		timeEnd = param[0].time + C_Game_SecondsInDay; // next day
		_time += C_Game_SecondsInDay;
	}
	else
	{
		for (uint32 i = last; i >= 0; i--)
		{
			if (_time >= param[i].time)
			{
				parBegin = param[i].value;
				timeBegin = param[i].time;

				if (i == last) // if current is last, then interpolate with first
				{
					parEnd = param[0].value;
					timeEnd = param[0].time + C_Game_SecondsInDay;
				}
				else
				{
					parEnd = param[i + 1].value;
					timeEnd = param[i + 1].time;
				}
				break;
			}
		}
	}

	float tt = (float)(_time - timeBegin) / (float)(timeEnd - timeBegin);
	return parBegin * (1.0f - tt) + (parEnd * tt);
}
