#include "stdafx.h"

// Include
#include "ParticleSystem.h"

// General
#include "ParticleEmitters.h"

mat4 CalcSpreadMatrix(float Spread1, float Spread2, float w, float l)
{
    Random random;

	int i, j;
	float a[2], c[2], s[2];
	mat4 Temp;

	mat4 SpreadMat = mat4(1.0f);

	a[0] = random.Range(-Spread1, Spread1) / 2.0f;
	a[1] = random.Range(-Spread2, Spread2) / 2.0f;

	/*SpreadMat.m[0][0]*=l;
	SpreadMat.m[1][1]*=l;
	SpreadMat.m[2][2]*=w;*/

	for (i = 0; i < 2; i++)
	{
		c[i] = cos(a[i]);
		s[i] = sin(a[i]);
	}

	Temp = mat4(1.0f);
	Temp[1][1] = c[0];
	Temp[2][1] = s[0];
	Temp[2][2] = c[0];
	Temp[1][2] = -s[0];

	SpreadMat = SpreadMat*Temp;

	Temp = mat4(1.0f);
	Temp[0][0] = c[1];
	Temp[1][0] = s[1];
	Temp[1][1] = c[1];
	Temp[0][1] = -s[1];

	SpreadMat = SpreadMat*Temp;

	float Size = abs(c[0])*l + abs(s[0])*w;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			SpreadMat[i][j] *= Size;

	return SpreadMat;
}

ParticleEmitter::ParticleEmitter(CM2_ParticleSystem& ParticleSystem)
	: m_ParticleSystem(ParticleSystem)
{}
