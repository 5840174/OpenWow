#pragma once

struct CM2_ParticleObject
{
	glm::vec3 pos;
	glm::vec3 speed;
	glm::vec3 down;
	glm::vec3 origin;
	glm::vec3 dir;

	float size, life, maxlife;

	glm::vec4 color;
};
