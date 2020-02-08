#include "stdafx.h"

// General
#include "M2_Part_Camera.h"

CM2_Part_Camera::CM2_Part_Camera(std::shared_ptr<IFile> f, const SM2_Camera& _proto, cGlobalLoopSeq global)
{
	nearclip = _proto.near_clip;
	farclip = _proto.far_clip;

	m_PositionBase = Fix_XZmY( _proto.position_base);
	m_TargetBase = Fix_XZmY(_proto.target_position_base);

	tPos.init(_proto.positions, f, global, Fix_XZmY);
	tTarget.init(_proto.target_position, f, global, Fix_XZmY);

	tRoll.init(_proto.roll, f, global);
	//fov = _proto.fov / sqrtf(1.0f + powf(m_VideoSettings->aspectRatio, 2.0f));;
}

void CM2_Part_Camera::calc(uint32 time, uint32 globalTime)
{
	if (tPos.uses(0))
	{
		pResult = tPos.getValue(0, time, globalTime);
	}

	if (tTarget.uses(0))
	{
		tResult = tTarget.getValue(0, time, globalTime);
	}

	if (tRoll.uses(0))
	{
		rollResult = tRoll.getValue(0, time, globalTime) / (glm::pi<float>() * 180.0f);
	}
}

void CM2_Part_Camera::setup(cvec3 _startPoint, float rotate)
{
	vec3 u(0, 1, 0);

	vec3 pp = m_PositionBase + pResult;
	vec3 tt = m_TargetBase + tResult;

	// TODO
	//_Render->getCamera()->setupViewParams(fov, m_VideoSettings.aspectRatio, nearclip, farclip);
	//_Render->getCamera()->Position = pp;
	//_Render->getCamera()->setViewMatrix(glm::lookAt(pp, tt, u));
}

void CM2_Part_Camera::getParams(vec3* _position, vec3* _target, float* _fov, float* _nearPlane, float* _farPlane)
{
	*_position = m_PositionBase + pResult;
	*_target = m_TargetBase + tResult;
	*_fov = fov;
	*_nearPlane = nearclip;
	*_farPlane = farclip;
}
