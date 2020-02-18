#pragma once

#include "DayNightPhase.h"

class DayNightCycle
{
public:
	DayNightCycle(IBaseManager& BaseManager);
	virtual ~DayNightCycle();

	DayNightPhase getPhase(uint32 _gameTime);

private:
	std::vector<DayNightPhase> dayNightPhases;
};
