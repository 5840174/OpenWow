#pragma once

struct SGPUM2Particle
{
	SGPUM2Particle()
		: Active(false)
	{}

	bool Active;
	glm::vec3 pos;
	glm::vec3 speed;
	glm::vec3 down;
	glm::vec3 creationPoint;
	glm::vec3 dir;

	float size, currentTime, maxTime;

	glm::vec4 color;
	int tile;

	SGPUParticle ToGPUParticle() const
	{
		return SGPUParticle(pos, ColorRGBA(color.r, color.g, color.b, color.a), glm::vec2(size));
	}
};
