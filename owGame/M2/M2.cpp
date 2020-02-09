#include "stdafx.h"

// Include
#include "M2_Base_Instance.h"
#include "M2_Builder.h"
#include "M2_Skin_Builder.h"

// General
#include "M2.h"

M2::M2(const std::string& name) :
	m_FileName(name),
	m_UniqueName(""),

	m_Materials(nullptr),
	m_Miscellaneous(nullptr),
	m_Skeleton(nullptr),

	// Loops and sequences
	m_IsAnimated(false),

	// Vertices
	m_IsContainGeom(false)
{
	//Log::Info("M2[%s]: Loading...", m_FileName.c_str());
}

void M2::CreateInsances(ISceneNode3D* _parent) const
{
	for (auto& it : m_Skins)
	{
		_parent->GetComponent<IMeshComponent3D>()->AddMesh(it);
		break;
	}
}

void M2::update(double _time, double _dTime)
{
	if (m_Miscellaneous)
	{
		m_Miscellaneous->update(_time, _dTime);
	}
}

void M2::calc(uint16 _animationIndex, uint32 _time, uint32 globalTime, cmat4 _viewMatrix, cmat4 _worldMatrix)
{
	if (m_Skeleton)
	{
		m_Skeleton->calc(_animationIndex, _time, globalTime, _viewMatrix, _worldMatrix);
	}

	if (m_Materials)
	{
		m_Materials->calc(_animationIndex, _time, globalTime);
	}

	if (m_Miscellaneous)
	{
		m_Miscellaneous->calc(_animationIndex, _time, globalTime, _worldMatrix);
	}
}
