#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_Camera.h"

SM2_Part_Camera_Wrapper::SM2_Part_Camera_Wrapper(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Camera& M2Camera)
	: m_M2Object(M2Object)
{
	nearclip = M2Camera.near_clip;
	farclip = M2Camera.far_clip;

	m_PositionBase = Fix_XZmY(M2Camera.position_base);
	m_TargetBase = Fix_XZmY(M2Camera.target_position_base);

	tPos.Initialize(M2Camera.positions, File, Fix_XZmY);
	tTarget.Initialize(M2Camera.target_position, File, Fix_XZmY);

	tRoll.Initialize(M2Camera.roll, File);
	//fov = M2Camera.fov / sqrtf(1.0f + powf(m_VideoSettings->aspectRatio, 2.0f));;
}

SM2_Part_Camera_Wrapper::~SM2_Part_Camera_Wrapper()
{
}

void SM2_Part_Camera_Wrapper::calc(uint32 time, uint32 globalTime)
{
	if (tPos.IsUsesBySequence(0))
	{
		pResult = tPos.GetValue(0, time, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
	}

	if (tTarget.IsUsesBySequence(0))
	{
		tResult = tTarget.GetValue(0, time, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
	}

	if (tRoll.IsUsesBySequence(0))
	{
		rollResult = tRoll.GetValue(0, time, m_M2Object.getSkeleton().getGlobalLoops(), globalTime) / (glm::pi<float>() * 180.0f);
	}
}

void SM2_Part_Camera_Wrapper::setup(const glm::vec3& _startPoint, float rotate)
{
	glm::vec3 u(0, 1, 0);

	glm::vec3 pp = m_PositionBase + pResult;
	glm::vec3 tt = m_TargetBase + tResult;

	// TODO
	//_Render->getCamera()->setupViewParams(fov, m_VideoSettings.aspectRatio, nearclip, farclip);
	//_Render->getCamera()->Position = pp;
	//_Render->getCamera()->setViewMatrix(glm::lookAt(pp, tt, u));
}

void SM2_Part_Camera_Wrapper::getParams(glm::vec3* _position, glm::vec3* _target, float* _fov, float* _nearPlane, float* _farPlane)
{
	*_position = m_PositionBase + pResult;
	*_target = m_TargetBase + tResult;
	*_fov = fov;
	*_nearPlane = nearclip;
	*_farPlane = farclip;
}
