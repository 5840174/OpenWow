#include "stdafx.h"

// Include
#include "M2_Builder.h"
#include "M2_Skin_Builder.h"

// General
#include "M2.h"



M2::M2(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::string& name) 
	: m_FileName(name)
	, m_UniqueName("")

	// Loops and sequences
	, m_IsAnimated(false)

	// Vertices
	, m_IsContainGeom(false)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
{
	m_Builder = std::make_unique<CM2_Builder>(BaseManager, RenderDevice, this);
	m_Builder->PreLoad();
	//Log::Info("M2[%s]: Loading...", m_FileName.c_str());
}

M2::~M2()
{
}

void M2::CreateInsances(const std::shared_ptr<ISceneNode3D>& Parent) const
{
	for (auto& it : m_Skins)
	{
		Parent->GetComponent<IMeshComponent3D>()->AddMesh(it);
		break;
	}
}

bool M2::Load()
{
	if (! m_Builder->Load())
		return false;

	m_Builder.reset();

	return true;
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
