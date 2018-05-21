#pragma once

#include "OpenGLAdapter.h"

class Engine
{
public:
	Engine(IOpenGLAdapter* _OpenGLAdapter, int argumentCount, char* arguments[]);
	~Engine();

	IOpenGLAdapter* GetAdapter() { return m_OpenGLAdapter; }

	bool Tick();

	string GetArgument(uint8 index) 
	{ 
		if (m_Arguments.size() > (index + 1))
		{
			return m_Arguments[index + 1];
		}
		else
		{
			return "";
		}
	}
	
	uint32 GetFPS() const { return framesPerSecond; }

private:
	vector<string> m_Arguments;
	bool needExit;


	uint32 framesCounter, framesPerSecond;
	double framesTimer;

	uint32 t;
	uint32 last_t;
	uint32 _time;

	IOpenGLAdapter* m_OpenGLAdapter;
};
