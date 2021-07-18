#include "stdafx.h"

// General
#include "WMOHelper.h"

namespace
{
	uint32 to_wmo_liquid(const SWMOGroup_MOGP& Header, int x)
	{
		DBC_LIQUIDTYPE_Type basic = (DBC_LIQUIDTYPE_Type)(x & 3);
		switch (basic)
		{
			case DBC_LIQUIDTYPE_Type::water:
				return (Header.flags.IS_NOT_WATER_BUT_OCEAN) ? 14 : 13;
			case DBC_LIQUIDTYPE_Type::ocean:
				return 14;
			case DBC_LIQUIDTYPE_Type::magma:
				return 19;
			case DBC_LIQUIDTYPE_Type::slime:
				return 20;
		}

		throw CException("Unexpected behaviour");
	}


}

/*void AttenTransVerts(WMO* mapObj, CWMOGroup* mapObjGroup)
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


const DBC_LiquidTypeRecord* WMOGroupResolveLiquidType(const CDBCStorage* DBCStorage, const SWMO_MOHD& WMOHeader, const SWMOGroup_MOGP& WMOGroupHeader)
{
	uint32 liquid_type = UINT32_MAX;

	if (WMOHeader.flags.use_liquid_type_dbc_id != 0)
	{
		if (WMOGroupHeader.liquidType < 21)
		{
			liquid_type = to_wmo_liquid(WMOGroupHeader, WMOGroupHeader.liquidType - 1);
		}
		else
		{
			liquid_type = WMOGroupHeader.liquidType;
		}
	}
	else
	{
		if (WMOGroupHeader.liquidType == 15 /*Liquid green lava*/)
		{
			//return nullptr;
			liquid_type = 1;  // use to_wmo_liquid(SMOLTile->liquid) ? It seems to work alright.
		}
		else
		{
			if (WMOGroupHeader.liquidType < 20)
			{
				liquid_type = to_wmo_liquid(WMOGroupHeader, WMOGroupHeader.liquidType);
			}
			else
			{
				liquid_type = WMOGroupHeader.liquidType + 1;
			}
		}
	}

	return DBCStorage->DBC_LiquidType()[liquid_type];
}


void WMOGroupFixColors(const SWMOGroup_MOGP& WMOGroupHeader, CBgra* mocv, uint32 mocv_count, const SWMOGroup_MOBA* WMOGroupBatchDefinitions)
{
	int32_t intBatchStart = 0;

	if (WMOGroupHeader.transBatchCount > 0)
		intBatchStart = WMOGroupBatchDefinitions[(uint16)WMOGroupHeader.transBatchCount].vertexStart;

	if (mocv_count > 0)
	{
		for (int32_t i = 0; i < mocv_count; i++)
		{
			auto& color = mocv[i];

			// Int / ext batches
			if (i >= intBatchStart)
			{
				int32_t r = (color.r + (color.a * color.r / 64.0f)) / 2.0f;
				int32_t g = (color.g + (color.a * color.g / 64.0f)) / 2.0f;
				int32_t b = (color.b + (color.a * color.b / 64.0f)) / 2.0f;

				color.r = glm::min(r, 255);
				color.g = glm::min(g, 255);
				color.b = glm::min(b, 255);
				color.a = 255u;
			}
			else
			{
				color.r /= 2;
				color.g /= 2;
				color.b /= 2;
			}
		}
	}
}


void WMOGroupFixColors_2(const SWMO_MOHD& WMOHeader, const SWMOGroup_MOGP& WMOGroupHeader, CBgra* mocv, uint32 mocv_count, const SWMOGroup_MOBA* moba)
{
	uint32 begin_second_fixup = 0;
	if (WMOGroupHeader.transBatchCount)
	{
		begin_second_fixup = moba[(uint16)WMOGroupHeader.transBatchCount].vertexStart + 1;
	}

	if (WMOHeader.flags.lighten_interiors)
	{
		for (uint32 i = begin_second_fixup; i < mocv_count; ++i)
		{
			mocv[i].a = WMOGroupHeader.flags.IS_OUTDOOR ? 255 : 0;
		}
	}
	else
	{
		uint8 r = 0, g = 0, b = 0;

		if (WMOHeader.flags.skip_base_color == 0)
		{
			r = WMOHeader.ambColor.r;
			g = WMOHeader.ambColor.g;
			b = WMOHeader.ambColor.b;
		}

		for (uint32 mocv_index = 0; mocv_index < begin_second_fixup; ++mocv_index)
		{
			mocv[mocv_index].b -= b;
			mocv[mocv_index].g -= g;
			mocv[mocv_index].r -= r;

			float v38 = (float)mocv[mocv_index].a / 255.0f;

			float v11 = (float)mocv[mocv_index].b - v38 * (float)mocv[mocv_index].b;
			_ASSERT(v11 > -0.5f);
			_ASSERT(v11 < 255.5f);
			mocv[mocv_index].b = v11 / 2;

			float v13 = (float)mocv[mocv_index].g - v38 * (float)mocv[mocv_index].g;
			_ASSERT(v13 > -0.5f);
			_ASSERT(v13 < 255.5f);
			mocv[mocv_index].g = v13 / 2;

			float v14 = (float)mocv[mocv_index].r - v38 * (float)mocv[mocv_index].r;
			_ASSERT(v14 > -0.5f);
			_ASSERT(v14 < 255.5f);
			mocv[mocv_index].r = v14 / 2;
		}

		for (uint32 i = begin_second_fixup; i < mocv_count; ++i)
		{
			int32 v19 = (mocv[i].b * mocv[i].a) / 64 + mocv[i].b - b;
			mocv[i].b = std::min(255, std::max(v19 / 2, 0));

			int32 v30 = (mocv[i].g * mocv[i].a) / 64 + mocv[i].g - g;
			mocv[i].g = std::min(255, std::max(v30 / 2, 0));

			int32 v33 = (mocv[i].a * mocv[i].r) / 64 + mocv[i].r - r;
			mocv[i].r = std::min(255, std::max(v33 / 2, 0));

			mocv[i].a = WMOGroupHeader.flags.IS_OUTDOOR ? 0xFF : 0x00;
		}
	}
}


namespace
{
	const float cEpsilon = 0.001f;

	glm::vec3 AjustDelta(BoundingBox src, float coef)
	{
		const glm::vec3 d = (src.getMax() - src.getMin()) * coef;
		return d + src.getMin();
	}
}

void TraverseBsp(const std::vector<std::shared_ptr<CWMOGroup_Part_CollisionNode>>& CollisionNodes, int16 CollisionNodeIndex, BoundingBox Eyes, BoundingBox Box, void *(pAction)(std::shared_ptr<CWMOGroup_Part_CollisionNode>, void*), void* param)
{
	if (CollisionNodeIndex == -1 || CollisionNodeIndex >= CollisionNodes.size())
		return; // Skip. Out of bounds.

	const auto collisionNode = CollisionNodes.at(CollisionNodeIndex);
	if (collisionNode == nullptr)
		return;
	const auto collisitonNodeProto = collisionNode->GetProto();

	if (collisitonNodeProto.flags & SWMOGroup_MOBN::Flags::Flag_Leaf)
	{
		pAction(collisionNode, param);
		return;
	}

	const uint16 axis = collisitonNodeProto.flags & SWMOGroup_MOBN::Flags::Flag_AxisMask;
	const float distance = collisitonNodeProto.planeDist;
	const int16 negChild = collisitonNodeProto.negChild;
	const int16 posChild = collisitonNodeProto.posChild;

	if ((-cEpsilon <= (Eyes.getMin()[axis] - Box.getMin()[axis])) || (Eyes.getMax()[axis] - Box.getMin()[axis]) >= -cEpsilon)
	{
		if ((cEpsilon <= (Box.getMax()[axis] - Eyes.getMax()[axis])) || ((Box.getMax()[axis] - Eyes.getMin()[axis]) >= cEpsilon))
		{
			glm::vec3 boxPositiveMin = Box.getMin();
			boxPositiveMin[axis] = distance;
			const BoundingBox boxPositive(boxPositiveMin, Box.getMax());

			glm::vec3 boxNegativeMax = Box.getMax();
			boxNegativeMax[axis] = distance;
			const BoundingBox boxNegative(Box.getMin(), boxNegativeMax);

			float eyes_min_fdist = Eyes.getMin()[axis] - distance;
			float eyes_max_fdist = Eyes.getMax()[axis] - distance;

			if (((eyes_min_fdist >= -cEpsilon) && (eyes_min_fdist <= cEpsilon)) || ((eyes_max_fdist >= -cEpsilon) && (eyes_max_fdist <= cEpsilon)))
			{
				TraverseBsp(CollisionNodes, negChild, Eyes, boxNegative, pAction, param);
				TraverseBsp(CollisionNodes, posChild, Eyes, boxPositive, pAction, param);
			}
			else if (eyes_min_fdist < -cEpsilon && eyes_max_fdist < -cEpsilon)
			{
				TraverseBsp(CollisionNodes, negChild, Eyes, boxNegative, pAction, param);
			}
			else if (eyes_min_fdist > cEpsilon && eyes_max_fdist > cEpsilon)
			{
				TraverseBsp(CollisionNodes, posChild, Eyes, boxPositive, pAction, param);
			}
			else
			{
				float eyesmin_div_deltadist = (eyes_min_fdist / (eyes_min_fdist - eyes_max_fdist));
				glm::vec3 ajusted = AjustDelta(Eyes, eyesmin_div_deltadist);

				if (eyes_min_fdist <= 0.0f)
				{
					TraverseBsp(CollisionNodes, negChild, BoundingBox(Eyes.getMin(), ajusted), boxNegative, pAction, param);
					TraverseBsp(CollisionNodes, posChild, BoundingBox(ajusted, Eyes.getMax()), boxPositive, pAction, param);
				}
				else
				{
					TraverseBsp(CollisionNodes, negChild, BoundingBox(ajusted, Eyes.getMax()), boxNegative, pAction, param);
					TraverseBsp(CollisionNodes, posChild, BoundingBox(Eyes.getMin(), ajusted), boxPositive, pAction, param);
				}
			}
		}
	}
}