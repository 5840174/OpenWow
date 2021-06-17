#pragma once

#ifdef USE_M2_MODELS

struct CM2_ParticleObject
{
	CM2_ParticleObject()
		: Active(false)
	{}

	bool Active;
	glm::vec3 pos;
	glm::vec3 speed;
	glm::vec3 down;
	glm::vec3 origin;
	glm::vec3 dir;

	float size, currentTime, maxTime;

	glm::vec4 color;
	int tile;
};

#endif