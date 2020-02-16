#pragma once

#include "WMO.h"
#include "WMO_Group.h"

inline void FixColorVertexAlpha(WMO_Group* _group)
{
	/*uint32 begin_second_fixup = 0;
	if (_group->m_Header.batchCounts[0])
	{
		begin_second_fixup = *((uint16*)&_group->moba[(uint16)_group->m_Header.batchCounts[0]] - 2) + 1;
	}

	if (_group->GetWMOModel().m_Header.flags.lighten_interiors)
	{
		for (uint32 i = begin_second_fixup; i < _group->mocv_count; ++i)
		{
			_group->mocv[i].w = _group->m_Header.flags.IS_OUTDOOR ? 0xFF : 0x00;
		}
	}
	else
	{
		uint8 v35, v36, v37;

		if (_group->GetWMOModel().m_Header.flags.skip_base_color)
		{
			v35 = 0;
			v36 = 0;
			v37 = 0;
		}
		else
		{
			v35 = (_group->GetWMOModel().m_Header.ambColor >> 0) & 0xff;
			v37 = (_group->GetWMOModel().m_Header.ambColor >> 8) & 0xff;
			v36 = (_group->GetWMOModel().m_Header.ambColor >> 16) & 0xff;
		}

		for (uint32 mocv_index = 0; mocv_index < begin_second_fixup; ++mocv_index)
		{
			_group->mocv[mocv_index].x -= v36;
			_group->mocv[mocv_index].y -= v37;
			_group->mocv[mocv_index].z -= v35;

			float v38 = (float)_group->mocv[mocv_index].w / 255.0f;

			float v11 = (float)_group->mocv[mocv_index].x - v38 * (float)_group->mocv[mocv_index].x;
			_ASSERT(v11 > -0.5f);
			_ASSERT(v11 < 255.5f);
			_group->mocv[mocv_index].x = v11 / 2;

			float v13 = (float)_group->mocv[mocv_index].y - v38 * (float)_group->mocv[mocv_index].y;
			_ASSERT(v13 > -0.5f);
			_ASSERT(v13 < 255.5f);
			_group->mocv[mocv_index].y = v13 / 2;

			float v14 = (float)_group->mocv[mocv_index].z - v38 * (float)_group->mocv[mocv_index].z;
			_ASSERT(v14 > -0.5f);
			_ASSERT(v14 < 255.5f);
			_group->mocv[mocv_index++].z = v14 / 2;
		}

		for (uint32 i = begin_second_fixup; i < _group->mocv_count; ++i)
		{
			int32 v19 = (_group->mocv[i].x * _group->mocv[i].w) / 64 + _group->mocv[i].x - v36;
			_group->mocv[i].x = std::min(255, std::max(v19 / 2, 0));

			int32 v30 = (_group->mocv[i].y * _group->mocv[i].w) / 64 + _group->mocv[i].y - v37;
			_group->mocv[i].y = std::min(255, std::max(v30 / 2, 0));

			int32 v33 = (_group->mocv[i].w * _group->mocv[i].z) / 64 + _group->mocv[i].z - v35;
			_group->mocv[i].z = std::min(255, std::max(v33 / 2, 0));

			_group->mocv[i].w = _group->m_Header.flags.IS_OUTDOOR ? 0xFF : 0x00;
		}
	}*/
}

/*void AttenTransVerts(WMO* mapObj, WMO_Group* mapObjGroup)
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