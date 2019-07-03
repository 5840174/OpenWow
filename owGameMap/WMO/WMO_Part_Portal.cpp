#include "stdafx.h"

// Include
#include "WMO.h"
#include "WMO_Group.h"
#include "WMO_Base_Instance.h"

// General
#include "WMO_Part_Portal.h"

CWMO_Part_Portal::CWMO_Part_Portal(const std::weak_ptr<const CWMO> _parentWMO, const SWMO_PortalDef& _proto) :
	m_ParentWMO(_parentWMO),
	m_GrInner(-1),
	m_GrOuter(-1)
{
	m_StartVertex = _proto.startVertex;
	m_Count = _proto.count;
	_ASSERT(m_Count < 20);

	m_Plane.normal = Fix_XZmY(_proto.plane.normal);
	m_Plane.dist = _proto.plane.distance;

	m_Geom = _RenderDevice->CreateMesh();
	m_Geom->AddVertexBuffer(BufferBinding("POSITION", 0), m_ParentWMO.lock()->m_PortalVB);
}


	/*_Render->r.setCullMode(R_CullMode::RS_CULL_BACK);
	_Render->r.setFillMode(R_FillMode::RS_FILL_WIREFRAME);

	_Render->getTechniquesMgr()->Debug_Pass->Bind();
	{
		_Render->getTechniquesMgr()->Debug_Pass->setWorld(_worldMatrix);

		vec4 color;
		if (m_Plane.distToPoint(glm::inverse(_worldMatrix) * vec4(_Render->getCamera()->Position, 0)) > 0.0f)
			color = vec4(0.0f, 1.0f, 0.0f, 0.3f);
		else
			color = vec4(1.0f, 0.0f, 0.0f, 0.3f);

		//if (IsVisible())
		//	color = vec4(0.0f, 1.0f, 0.0f, 0.3f);
		//else
		//	color = vec4(1.0f, 0.0f, 0.0f, 0.3f);

		_Render->getTechniquesMgr()->Debug_Pass->SetColor4(color);

		_Render->r.setGeometry(m_Geom);
		_Render->r.draw(m_StartVertex, m_Count);
	}
	_Render->getTechniquesMgr()->Debug_Pass->Unbind();

	_Render->r.setFillMode(R_FillMode::RS_FILL_SOLID);
	_Render->r.setCullMode(R_CullMode::RS_CULL_NONE);*/


void CWMO_Part_Portal::setGroup(int32 _group, int16 side)
{
	if (side == 1)
	{
		m_GrInner = _group;
	}
	else if (side == -1)
	{
		m_GrOuter = _group;
	}
	else
	{
		fail1();
	}
}

bool CWMO_Part_Portal::IsVisible(std::shared_ptr<CWMO_Base_Instance> _localContr, const Plane* _planes, uint32 _planesCount) const
{
	if (_planes == nullptr || _planesCount == 0)
		return false;

	return !cullPolyByPlanes
	(
		_planes,
		_planesCount,
		_localContr->getVerts() + getStartVertex(),
		getCount()
	);
}

bool CWMO_Part_Portal::IsPositive(cvec3 _InvWorldCamera) const
{
	return m_Plane.distToPoint(_InvWorldCamera) > 0.0f;
}
