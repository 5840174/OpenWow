#pragma once

struct Particle
{
	vec3 pos;
	vec3 speed;
	vec3 down;
	vec3 origin;
	vec3 dir;

	vec3 corners[4];

	float size, life, maxlife;

	uint32 m_TileExists;

	vec4 color;
};
