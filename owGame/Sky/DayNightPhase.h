#pragma once

struct SDayNightPhase
{
	SDayNightPhase();
	SDayNightPhase(std::shared_ptr<IFile> f);
	SDayNightPhase(const SDayNightPhase& a, const SDayNightPhase& b, float r);

	float dayIntensity;
	ColorRGB dayColor;
	glm::vec3 dayDir;

	float nightIntensity;
	ColorRGB nightColor;
	glm::vec3 nightDir;
		
	float ambientIntensity;
	ColorRGB ambientColor;

	float fogDepth;
	float fogIntensity;
	ColorRGB fogColor;
};