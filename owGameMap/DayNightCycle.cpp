#include "stdafx.h"

// General
#include "DayNightCycle.h"

DayNightCycle::DayNightCycle(IBaseManager* BaseManager)
{
	std::shared_ptr<IFile> f = BaseManager->GetManager<IFilesManager>()->Open("World\\dnc.db");
	if (f == nullptr)
	{
		Log::Error("DayNightCycle[]: Can't init day-night cycle.");
        _ASSERT(false);
	}

	uint32 nFields1, nFields, d;

	// Header
    f->readBytes(&nFields, 4);
	f->readBytes(&nFields1, 4);
	_ASSERT(nFields == nFields1);
	_ASSERT(nFields == 25);

	// Field Descriptions
	uint32 magic0x53;
	f->readBytes(&magic0x53, 4);
	_ASSERT(magic0x53 == 0x53);

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

DayNightCycle::~DayNightCycle()
{
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