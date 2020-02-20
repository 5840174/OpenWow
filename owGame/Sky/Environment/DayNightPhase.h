#pragma once

class DayNightPhase
{
public:
	DayNightPhase();
	DayNightPhase(std::shared_ptr<IFile> f);
	DayNightPhase(DayNightPhase* a, DayNightPhase* b, float r);

public:
	float dayIntensity;
	vec3 dayColor;
	vec3 dayDir;

	float nightIntensity;
	vec3 nightColor;
	vec3 nightDir;
		
	float ambientIntensity;
	vec3 ambientColor;

	float fogDepth;
	float fogIntensity;
	vec3 fogColor;
};