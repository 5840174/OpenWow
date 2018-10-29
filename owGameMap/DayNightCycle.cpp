#include "stdafx.h"

// General
#include "DayNightCycle.h"

DayNightCycle::DayNightCycle()
{
	std::shared_ptr<IFile> f = GetManager<IFilesManager>()->Open("World\\dnc.db");
	if (f == nullptr)
	{
		Log::Error("DayNightCycle[]: Can't init day-night cycle.");
        fail1();
	}

	uint32 nFields1, nFields, d;

	// Header
    f->readBytes(&nFields, 4);
	f->readBytes(&nFields1, 4);
	assert1(nFields == nFields1);
	assert1(nFields == 25);

	// Field Descriptions
	uint32 magic0x53;
	f->readBytes(&magic0x53, 4);
	assert1(magic0x53 == 0x53);

	// Final offset
	f->readBytes(&d, 4); // d is now the final offset

	// Skip names
	f->seek(8 + nFields * 8);

	while (f->getPos() < d)
	{
		DayNightPhase ols(f);
		dayNightPhases.push_back(ols);
	}
}

DayNightPhase DayNightCycle::getPhase(uint32 _gameTime)
{
	uint32 hourA = _gameTime / 120;
	uint32 hourB = (hourA + 1) % 24;

	DayNightPhase* a = &dayNightPhases[hourA];
	DayNightPhase* b = &dayNightPhases[hourB];

	float r = static_cast<float>(_gameTime - (hourA * 120)) / 120.0f;

	return DayNightPhase(a, b, r);
}