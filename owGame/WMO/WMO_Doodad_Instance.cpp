#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "WMO_Doodad_Instance.h"

CWMO_Doodad_Instance::CWMO_Doodad_Instance(const M2& M2Object, const WMO_Group& WMOGroupObject, uint32 _index, const SWMO_Doodad_PlacementInfo & _placement)
	: CM2_Base_Instance(M2Object)
	, m_WMOGroupObject(WMOGroupObject)
	, m_Index(_index)
	, m_PortalVis(true)
{
	// CTransformComponent
	{
		SetTranslate(Fix_XZmY(_placement.position));
		SetRotationQuaternion(quat(_placement.orientation.w, -_placement.orientation.z, _placement.orientation.x, _placement.orientation.y));
		SetScale(vec3(_placement.scale, -_placement.scale, -_placement.scale));
	}

	/*if (_mdxObject->getFilename().find("LD_LIGHTSHAFT") != -1)
	{
		Log::Green("MODEL [%s] contains color [%f, %f, %f, %f]", _mdxObject->getFilename().c_str(), _placement.getColor().x, _placement.getColor().y, _placement.getColor().z, _placement.getColor().w);
	}*/

	//if (!m_ParentGroup->m_Header.flags.IS_OUTDOOR)
	{
		//m_DoodadColor = _placement.getColor();
	}
	//else
	{
		//m_DoodadColor = vec4(1.0f);
	}
	//m_Object->setDoodadColor(_placement.getColor());


}

CWMO_Doodad_Instance::~CWMO_Doodad_Instance()
{}

void CWMO_Doodad_Instance::Initialize()
{
	__super::Initialize();
}

void CWMO_Doodad_Instance::Accept(IVisitor* visitor)
{
	if (m_PortalVis)
	{
		return CM2_Base_Instance::Accept(visitor);
	}
}

#endif