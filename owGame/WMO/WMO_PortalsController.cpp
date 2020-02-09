#include "stdafx.h"



// Include
#include "WMO.h"
#include "WMO_Base_Instance.h"

// General
#include "WMO_PortalsController.h"

// Additional
#include "WMO_Group_Instance.h"

#ifndef WMO_DISABLE_PORTALS

CWMO_PortalsController::CWMO_PortalsController(const CWMO& WMOObject) 
	: m_WMOObject(WMOObject)
{
	for (auto& it : m_WMOObject.m_PortalReferences)
	{
		std::shared_ptr<CWMO_Part_Portal> portal = m_WMOObject.m_Portals[it.portalIndex];
		std::shared_ptr<WMO_Group> group = m_WMOObject.m_Groups[it.groupIndex];

		if (it.groupIndex == group->m_GroupIndex)
		{
			group->m_Portals.push_back(portal);
		}
		else
		{
			_ASSERT(false);
		}

		portal->setGroup(it.groupIndex, it.side);
	}
}

CWMO_PortalsController::~CWMO_PortalsController()
{
}

void CWMO_PortalsController::GetPolyFrustum(const vec3* poly, uint32 num_verts, Frustum* _frustum, vec3 eye, bool _isPositive)
{
	_ASSERT(_frustum != nullptr);

	Plane _portalPlanes[15];
	_ASSERT(num_verts < 15);

	for (uint32 i = 0; i < num_verts; i++)
	{
		vec3 v1 = poly[i];
		vec3 v2 = poly[(i + 1) % num_verts];

		if (_isPositive)
		{
			_portalPlanes[i] = Plane(eye, v1, v2);
		}
		else
		{
			_portalPlanes[i] = Plane(eye, v2, v1);
		}
	}

	_frustum->buildCustomFrustrum(_portalPlanes, num_verts);
}

void CWMO_PortalsController::Update(const CWMO_Base_Instance* _localContr, const ICameraComponent3D* _camera)
{
	// Reset all flags
	for (auto& group : _localContr->getGroupInstances())
	{
		group->SetPortalVisible(false);
		group->SetPortalCalculated(false);

#ifdef USE_M2_MODELS
		for (auto& doodad : group->getDoodadsInstances())
		{
			if (doodad)
			{
				doodad->setPortalVisibility(false);
			}
		}
#endif
	}

	vec3 _InvWorldCamera = _localContr->GetInverseWorldTransform() * vec4(_camera->GetTranslation(), 1.0f);

	bool insideIndoor = false;

	if (m_WMOObject.GetBounds().isPointInside(_InvWorldCamera))
	{
		for (const auto& group : _localContr->getGroupInstances())
		{
			if (!(group->GetComponent<CColliderComponent3D>()->GetBounds().isPointInside(_camera->GetTranslation())))
			{
				continue;
			}

			if (!group->getObject().m_Header.flags.HAS_COLLISION)
			{
				continue;
			}

			if (group->getObject().m_Header.flags.IS_OUTDOOR)
			{
				continue;
			}

			bool recurResult = Recur(_localContr, group, _camera, _InvWorldCamera, _camera->GetFrustum(), true);
			if (!recurResult)
			{
				continue;
			}

			if (group->getObject().m_Header.flags.IS_INDOOR)
			{
				insideIndoor = true;
			}
		}
	}

	//_ASSERT(insideOneAtLeast || !(m_ParentWMO->m_OutdoorGroups.empty()));

	// If we outside WMO, then get outdorr group
	//if (!insideIndoor)
	{
		for (auto& ogr : _localContr->getGroupOutdoorInstances())
		{
			Recur(_localContr, ogr, _camera, _InvWorldCamera, _camera->GetFrustum(), true);
		}
	}
}

bool CWMO_PortalsController::Recur(const CWMO_Base_Instance* _localContr, CWMO_Group_Instance* _group, const ICameraComponent3D* _camera, cvec3 _InvWorldCamera, const Frustum& _frustum, bool _isFirstIteration)
{
	if (_group == nullptr || _group->GetPortalCalculated())
	{
		return false;
	}

	if (_camera->GetFrustum().cullBox(_group->GetComponent<CColliderComponent3D>()->GetBounds()))
	{
		return false;
	}

	// Set visible for current
	_group->SetPortalVisible(true);
	_group->SetPortalCalculated(true);

#ifdef USE_M2_MODELS
	for (const auto& doodad : _group->getDoodadsInstances())
	{
		if (doodad && (_isFirstIteration || !_frustum.cullBox(doodad->GetComponent<IColliderComponent3D>()->GetBounds())))
		{
			doodad->setPortalVisibility(true);
		}
	}
#endif

	for (const auto& p : _group->getObject().m_Portals)
	{
		// If we don't see portal // TODO: Don't use it on first step
		if (_camera->GetFrustum().cullPoly(_localContr->getVerts() + p->getStartVertex(), p->getCount()))
		{
			continue;
		}

		// And we don't see portal from other portal
		if (!p->IsVisible(_localContr, _frustum.getPlanes(), _frustum.getPlanesCnt()))
		{
			continue;
		}

		bool isPositive = p->IsPositive(_InvWorldCamera);

		// Build camera-to-poratl frustum
		Frustum portalFrustum;
		GetPolyFrustum
		(
			_localContr->getVerts() + p->getStartVertex(),
			p->getCount(),
			&portalFrustum,
			_camera->GetTranslation(),
			isPositive
		);

		// Find attached to portal group
		CWMO_Group_Instance* groupInstance = nullptr;
		int32 groupIndex = isPositive ? p->getGrInner() : p->getGrOuter();
		if (groupIndex >= 0 && groupIndex < _localContr->getGroupInstances().size())
		{
			groupInstance = _localContr->getGroupInstances()[groupIndex];
		}

		Recur
		(
			_localContr,
			groupInstance,
			_camera,
			_InvWorldCamera,
			portalFrustum,
			false
		);
	}

	return true;
}

#endif
