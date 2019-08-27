#include "stdafx.h"

// General
#include "WMO_Doodad_Instance.h"

CWMO_Doodad_Instance::CWMO_Doodad_Instance(std::string _m2Name, const std::weak_ptr<const WMO_Group> _parentGroup, uint32 _index) :
	CM2_Base_Instance(_m2Name),
	m_ParentGroup(_parentGroup),
	m_Index(_index),
	m_PortalVis(true)
{
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

void CWMO_Doodad_Instance::Initialize(const SWMO_Doodad_PlacementInfo & _placement)
{
    // CTransformComponent
    std::shared_ptr<CTransformComponent3D> transformComponent = GetComponent<CTransformComponent3D>();
    {
        transformComponent->SetTranslate(Fix_XZmY(_placement.position));
        transformComponent->SetRotationQuaternion(quat(_placement.orientation.w, -_placement.orientation.z, _placement.orientation.x, _placement.orientation.y));
        transformComponent->SetScale(vec3(_placement.scale, -_placement.scale, -_placement.scale));
    }
}

bool CWMO_Doodad_Instance::Accept(IVisitor* visitor)
{
	if (!m_PortalVis)
	{
		return false;
	}

	return CM2_Base_Instance::Accept(visitor);
}
