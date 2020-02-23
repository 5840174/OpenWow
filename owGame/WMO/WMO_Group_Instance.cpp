#include "stdafx.h"

// Include
#include "WMO_Base_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

// General
#include "WMO_Group_Instance.h"

CWMO_Group_Instance::CWMO_Group_Instance(const std::shared_ptr<WMO_Group>& WMOGroupObject) 
	: CLoadableObject(WMOGroupObject)
	, m_PortalsVis(true)
	, m_Calculated(false)
	, m_WMOGroupObject(*WMOGroupObject)
{
}

CWMO_Group_Instance::~CWMO_Group_Instance()
{
}




//
// CLoadableObject
//
bool CWMO_Group_Instance::Load()
{
	m_WMOGroupObject.CreateInsances(shared_from_this());

	return true;
}



//
// IPortalRoom
//
void CWMO_Group_Instance::AddPortal(const std::shared_ptr<IPortal>& Portal)
{
	m_RoomPortals.push_back(Portal);
}

const std::vector<std::shared_ptr<IPortal>>& CWMO_Group_Instance::GetPortals() const
{
	return m_RoomPortals;
}

void CWMO_Group_Instance::AddRoomObject(const std::weak_ptr<IPortalRoomObject>& RoomObject)
{
	m_PortalRoomObjects.push_back(RoomObject);
}

const std::vector<std::weak_ptr<IPortalRoomObject>>& CWMO_Group_Instance::GetRoomObjects() const
{
	return m_PortalRoomObjects;
}

void CWMO_Group_Instance::Reset()
{
	SetVisibilityState(false);
	SetCalculatedState(false);

	for (const auto& roomObjectPtr : GetRoomObjects())
		if (auto roomObject = roomObjectPtr.lock())
			roomObject->SetVisibilityState(false);
}

BoundingBox CWMO_Group_Instance::GetBoundingBox() const
{
	return GetComponent<IColliderComponent3D>()->GetWorldBounds();
}

void CWMO_Group_Instance::SetVisibilityState(bool Value)
{
	m_PortalsVis = Value;
}

void CWMO_Group_Instance::SetCalculatedState(bool Value)
{
	m_Calculated = Value;
}

bool CWMO_Group_Instance::IsCalculated() const
{
	return m_Calculated;
}



//
// CWMO_Group_Instance
//
void CWMO_Group_Instance::CreatePortals(const std::shared_ptr<CWMO_Base_Instance>& BaseInstance)
{
	for (const auto& p : m_WMOGroupObject.GetPortals())
	{
		std::shared_ptr<CWMO_Group_Instance> roomInnerInstance = nullptr;
		if (p.getGrInner() != -1)
		{
			roomInnerInstance = BaseInstance->getGroupInstances()[p.getGrInner()].lock();
			_ASSERT(roomInnerInstance != nullptr);
		}

		std::shared_ptr<CWMO_Group_Instance> roomOuterInstance = nullptr;
		if (p.getGrOuter() != -1)
		{
			roomOuterInstance = BaseInstance->getGroupInstances()[p.getGrOuter()].lock();
			_ASSERT(roomOuterInstance != nullptr);
		}

		std::vector<glm::vec3> portalsVertices = p.GetVertices();
		for (auto& pv : portalsVertices)
			pv = BaseInstance->GetWorldTransfom() * glm::vec4(pv, 1.0f);

		glm::vec4 O = glm::vec4(p.getPlane().normal * p.getPlane().dist, 1.0f);
		glm::vec4 N = glm::vec4(p.getPlane().normal, 0.0f);

		O = BaseInstance->GetWorldTransfom() * O;
		N = glm::transpose(glm::inverse(BaseInstance->GetWorldTransfom())) * N;

		//Plane plane;
		//plane.normal = N.xyz();
		//plane.dist = glm::dot(O.xyz(), N.xyz());

		Plane plane(portalsVertices[0], portalsVertices[1], portalsVertices[2]);
		AddPortal(std::make_shared<CWMOPortalInstance>(roomInnerInstance, roomOuterInstance, portalsVertices, plane));
	}
}



//
// SceneNode3D
//
std::string CWMO_Group_Instance::GetName() const
{
	return "WMOGroup '" + m_WMOGroupObject.m_GroupName + "'";
}

void CWMO_Group_Instance::Initialize()
{


	BoundingBox bbox = m_WMOGroupObject.m_Bounds;
	bbox.calculateCenter();
	GetComponent<IColliderComponent3D>()->SetBounds(bbox);
	GetComponent<CColliderComponent3D>()->SetDebugDrawMode(false);
}

void CWMO_Group_Instance::Accept(IVisitor* visitor)
{
	if (m_PortalsVis)
	{
		SceneNode3D::Accept(visitor);
	}	
}
