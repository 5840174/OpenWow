#include "stdafx.h"

// Include
#include "WMO.h"
#include "WMO_Base_Instance.h"

// General
#include "WMO_PortalsController.h"

// Additional
#include "WMO_Group_Instance.h"

CWMO_PortalsController::CWMO_PortalsController(const std::weak_ptr<const WMO> _parentWMO) :
	m_ParentWMO(_parentWMO)
{
	std::shared_ptr<const WMO> ParentWMO = m_ParentWMO.lock();
	assert1(ParentWMO != nullptr);

	for (auto& it : ParentWMO->m_PortalReferences)
	{
		std::shared_ptr<CWMO_Part_Portal> portal = ParentWMO->m_Portals[it.portalIndex];
		std::shared_ptr<WMO_Group> group = ParentWMO->m_Groups[it.groupIndex];

		if (it.groupIndex == group->m_GroupIndex)
		{
			group->m_Portals.push_back(portal);
		}
		else
		{
			fail1();
		}

		portal->setGroup(it.groupIndex, it.side);
	}
}

void CWMO_PortalsController::GetPolyFrustum(const vec3* poly, uint32 num_verts, Frustum* _frustum, vec3 eye, bool _isPositive)
{
	assert1(_frustum != nullptr);

	Plane _portalPlanes[15];
	assert1(num_verts < 15);

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

void CWMO_PortalsController::Update(std::shared_ptr<CWMO_Base_Instance> _localContr, const Camera& _camera)
{
	// Reset all flags
	for (auto& group : _localContr->getGroupInstances())
	{
		group->m_PortalsVis = false;
		group->m_Calculated = false;

		for (auto& doodad : group->getDoodadsInstances())
		{
			if (doodad)
			{
				doodad->setPortalVisibility(false);
			}
		}
	}

	vec3 _InvWorldCamera = _localContr->GetInverseWorldTransform() * vec4(_camera.GetTranslation(), 1.0f);

	bool insideIndoor = false;

	if (m_ParentWMO.lock()->GetBounds().isPointInside(_InvWorldCamera))
	{
		for (auto& group : _localContr->getGroupInstances())
		{
			if (!(group->GetBounds().isPointInside(_camera.GetTranslation())))
			{
				continue;
			}

			if (!group->getObject()->m_Header.flags.HAS_COLLISION)
			{
				continue;
			}

			if (group->getObject()->m_Header.flags.IS_OUTDOOR)
			{
				continue;
			}

			bool recurResult = Recur(_localContr, group, _camera, _InvWorldCamera, _camera.GetFrustum(), true);
			if (!recurResult)
			{
				continue;
			}

			if (group->getObject()->m_Header.flags.IS_INDOOR)
			{
				insideIndoor = true;
			}
		}
	}

	//assert1(insideOneAtLeast || !(m_ParentWMO->m_OutdoorGroups.empty()));

	// If we outside WMO, then get outdorr group
	//if (!insideIndoor)
	{
		for (auto& ogr : _localContr->getGroupOutdoorInstances())
		{
			Recur(_localContr, ogr, _camera, _InvWorldCamera, _camera.GetFrustum(), true);
		}
	}
}

bool CWMO_PortalsController::Recur(std::shared_ptr<CWMO_Base_Instance> _localContr, std::shared_ptr<CWMO_Group_Instance> _group, const Camera& _camera, cvec3 _InvWorldCamera, const Frustum& _frustum, bool _isFirstIteration)
{
	if (_group == nullptr || _group->m_Calculated)
	{
		return false;
	}

	if (_camera.GetFrustum().cullBox(_group->GetBounds()))
	{
		return false;
	}

	// Set visible for current
	_group->m_PortalsVis = true;
	_group->m_Calculated = true;
	for (auto& doodad : _group->getDoodadsInstances())
	{
		if (doodad && (_isFirstIteration || !_frustum.cullBox(doodad->GetBounds())))
		{
			doodad->setPortalVisibility(true);
		}
	}

	for (auto& p : _group->getObject()->m_Portals)
	{
		// If we don't see portal // TODO: Don't use it on first step
		if (_camera.GetFrustum().cullPoly(_localContr->getVerts() + p->getStartVertex(), p->getCount()))
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
			_camera.GetTranslation(),
			isPositive
		);

		// Find attached to portal group
		std::shared_ptr<CWMO_Group_Instance> groupInstance = nullptr;
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