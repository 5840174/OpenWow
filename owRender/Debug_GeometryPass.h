#pragma once

#include "technique.h"

class Debug_GeometryPass : public Technique
{
public:
	Debug_GeometryPass() : Technique("shaders/Debug/Debug") 
    {
        gColor = getLocation("gColor");
    }

	inline void SetColor4(cvec4 _color)
	{
		setVec4(gColor, _color);
	}

private:
    int32 gColor;
};