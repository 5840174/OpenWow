#pragma once

#ifdef USE_WMO_MODELS

#include "WMO.h"
#include "WMO_Group.h"



/*void AttenTransVerts(WMO* mapObj, CWMO_Group* mapObjGroup)
{
	//mapObjGroup->field_98 |= 1u;

	if (!mapObjGroup->m_Header.batchCounts[0])
	{
		return;
	}

	for (std::size_t vertex_index(0)
		; vertex_index < (*((unsigned __int16 *)&mapObjGroup->moba[(unsigned __int16)mapObjGroup->m_Header.batchCounts[0]] - 2) + 1)
		; ++vertex_index
		)
	{
		float opacity_accum(0.0);

		for (std::size_t portal_ref_index(mapObjGroup->mogp->mopr_index)
			; portal_ref_index < (mapObjGroup->mogp->mopr_index + mapObjGroup->mogp->mopr_count)
			; ++portal_ref_index
			)
		{
			SMOPortalRef const& portalRef(mapObj->mopr[portal_ref_index]);
			SMOPortal const& portal(mapObj->mopt[portalRef.portalIndex]);
			C3Vector const& vertex(&_group->movt[vertex_index]);

			float const portal_to_vertex(distance(portal.plane, vertex));

			C3Vector vertex_to_use(vertex);

			if (portal_to_vertex > 0.001 || portal_to_vertex < -0.001)
			{
				C3Ray ray(C3Ray::FromStartEnd(
					vertex
					, vertex
					+ (portal_to_vertex > 0 ? -1 : 1) * portal.plane.normal
					, 0
				)
				);
				NTempest::Intersect(ray, &portal.plane, 0LL, &vertex_to_use, 0.0099999998);
			}

			float distance_to_use;

			if (NTempest::Intersect(vertex_to_use
				, &mapObj->mopv[portal.base_index]
				, portal.index_count
				, C3Vector::MajorAxis(portal.plane.normal)
			)
				)
			{
				distance_to_use = portalRef.side * distance(portal.plane, vertex);
			}
			else
			{
				distance_to_use = NTempest::DistanceFromPolygonEdge
				(vertex, &mapObj->mopv[portal.base_index], portal.index_count);
			}

			if (mapObj->mogi[portalRef.group_index].flags & 0x48)
			{
				float v25(distance_to_use >= 0.0 ? distance_to_use / 6.0f : 0.0f);
				if ((1.0 - v25) > 0.001)
				{
					opacity_accum += 1.0 - v25;
				}
			}
			else if (distance_to_use > -1.0)
			{
				opacity_accum = 0.0;
				if (distance_to_use < 1.0)
				{
					break;
				}
			}
		}

		float const opacity(opacity_accum > 0.001 ? std::min(1.0f, opacity_accum) : 0.0f);

		//! \note all assignments asserted to be > -0.5 && < 255.5f
		C4Vec& color(mapObjGroup->mocv[vertex_index]);
		color.x = ((127.0f - color.x) * opacity) + color.x;
		color.y = ((127.0f - color.y) * opacity) + color.y;
		color.z = ((127.0f - color.z) * opacity) + color.z;
		color.w = opacity * 255.0;
	}
}*/

#endif