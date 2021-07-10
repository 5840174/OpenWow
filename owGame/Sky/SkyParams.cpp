#include "stdafx.h"

// General
#include "SkyParams.h"

SSkyParams::SSkyParams()
{
	Clear();
}

SSkyParams::~SSkyParams()
{}


const SSkyParams& SSkyParams::operator+=(const SSkyParams& Other)
{
	for (uint8 i = 0; i < ESkyColors::SKY_COLOR_COUNT; i++)
		Colors[i] = Colors[i] + Other.Colors[i];

	for (uint8 i = 0; i < ESkyFogs::SKY_FOG_COUNT; i++)
		Fogs[i] += Other.Fogs[i];

	for (uint8 i = 0; i < ESkyWaterAlpha::SKY_COUNT; i++)
		WaterAplha[i] += Other.WaterAplha[i];

	Glow += Other.Glow;
	return *this;
}

const SSkyParams& SSkyParams::operator*=(float Weight)
{
	for (uint8 i = 0; i < ESkyColors::SKY_COLOR_COUNT; i++)
		Colors[i] = Colors[i] * Weight;

	for (uint8 i = 0; i < ESkyFogs::SKY_FOG_COUNT; i++)
		Fogs[i] *= Weight;

	for (uint8 i = 0; i < ESkyWaterAlpha::SKY_COUNT; i++)
		WaterAplha[i] *= Weight;

	Glow *= Weight;

	return *this;
}

void SSkyParams::Clear()
{
	for (uint8 i = 0; i < ESkyColors::SKY_COLOR_COUNT; i++)
		Colors[i] = ColorRGB(0.0f, 0.0f, 0.0f);

	for (uint8 i = 0; i < ESkyFogs::SKY_FOG_COUNT; i++)
		Fogs[i] = 0.0f;

	for (uint8 i = 0; i < ESkyWaterAlpha::SKY_COUNT; i++)
		WaterAplha[i] = 0.0f;

	Glow = 0.0f;
}
