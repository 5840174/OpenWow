#pragma once

class DayNightPhase
{
public:
	DayNightPhase();
	DayNightPhase(std::shared_ptr<IFile> f);
	DayNightPhase(DayNightPhase* a, DayNightPhase* b, float r);

public:
	float dayIntensity;
	glm::vec3 dayColor;
	glm::vec3 dayDir;

	float nightIntensity;
	glm::vec3 nightColor;
	glm::vec3 nightDir;
		
	float ambientIntensity;
	glm::vec3 ambientColor;

	float fogDepth;
	float fogIntensity;
	glm::vec3 fogColor;
};