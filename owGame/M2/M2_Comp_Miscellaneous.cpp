#include "stdafx.h"

// Include
#include "M2_Builder.h"

// General
#include "M2_Comp_Miscellaneous.h"

CM2_Comp_Miscellaneous::CM2_Comp_Miscellaneous() :
	m_HasMisc(false)
{
}

void CM2_Comp_Miscellaneous::update(double _time, double _dTime)
{
	for (auto& it : particleSystems)
	{
		it->update(_time, _dTime);
	}
}

void CM2_Comp_Miscellaneous::calc(uint16 anim, uint32 time, uint32 globalTime, cmat4 _worldMat)
{
	for (auto& it : m_Lights)
	{
		it->setup(anim, time, globalTime);
	}

	for (auto& it : m_Cameras)
	{
		it->calc(time, globalTime);
	}

	for (auto& it : m_RibbonEmitters)
	{
		it->setup(anim, time, globalTime, _worldMat);
	}

	for (auto& it : particleSystems)
	{
		it->setup(anim, time, globalTime);
	}
}

void CM2_Comp_Miscellaneous::render(cmat4 _worldMat)
{

	for (auto& it : m_Attachments)
	{
		it->render(_worldMat);
	}

	for (auto& it : m_RibbonEmitters)
	{
		it->Render(_worldMat);
	}

	for (auto& it : particleSystems)
	{
		it->Render3D(_worldMat);
	}
}
