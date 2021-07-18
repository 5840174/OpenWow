#include "stdafx.h"

// General
#include "WMOHelper.h"

namespace
{
	const float cEpsilon = 0.01f;

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

	void MergeBox(glm::vec3(&result)[2], float  *box1, float  *box2)
	{
		result[0][0] = box1[0];
		result[0][1] = box1[1];
		result[0][2] = box1[2];

		result[1][0] = box2[0];
		result[1][1] = box2[1];
		result[1][2] = box2[2];
	}

	void AjustDelta(glm::vec3(&src)[2], float *dst, float coef)
	{
		float d1 = (src[1][0] - src[0][0]) * coef;// delta x
		float d2 = (src[1][1] - src[0][1]) * coef;// delta y
		float d3 = (src[1][2] - src[0][2]) * coef;// delta z
		dst[1] = d1 + src[0][1];
		dst[0] = d2 + src[0][0];
		dst[2] = d3 + src[0][2];
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

void TraverseBsp(int iNode, glm::vec3(&pEyes)[2], glm::vec3(&pBox)[2], void *(pAction)(SWMOGroup_MOBN*, void*), void* param)
{
	glm::vec3 newEyes[2];
	glm::vec3 ajusted;
	SWMOGroup_MOBN *pNode = nullptr; // &m_tNode[iNode];

	if (pNode == nullptr)
		return;

	if (pNode->flags & SWMOGroup_MOBN::Flags::Flag_Leaf)
	{
		if (pAction == nullptr)
		{
			//RenderGeometry(GetEngine3DInstance(), pNode);
			return;
		}
		else
		{
			pAction(pNode, param);
		}
	}

	int plane = pNode->flags & SWMOGroup_MOBN::Flags::Flag_AxisMask;

	float eyesmin_boxmin = pEyes[0][plane] - pBox[0][plane];
	float boxmax_eyesmax = pBox[1][plane] - pEyes[1][plane];

	if (false == ((-cEpsilon < eyesmin_boxmin) | (-cEpsilon == eyesmin_boxmin)) && false == ((pEyes[1][plane] - pBox[0][plane]) >= -cEpsilon) &&
		false == ((cEpsilon < boxmax_eyesmax) | (cEpsilon == boxmax_eyesmax)) && false == ((pBox[1][plane] - pEyes[0][plane]) >= cEpsilon)
		)
		return;

	glm::vec3 tBox1[2];
	memmove(tBox1, pBox, sizeof(pBox));
	tBox1[0][plane] = pNode->planeDist;

	glm::vec3 tBox2[2];
	memmove(tBox2, pBox, sizeof(pBox));
	tBox2[1][plane] = pNode->planeDist;

	float eyes_min_fdist = (pEyes[0][plane]) - pNode->planeDist;
	float eyes_max_fdist = (pEyes[1][plane]) - pNode->planeDist;

	if (((eyes_min_fdist >= -cEpsilon) && (eyes_min_fdist <= cEpsilon)) || ((eyes_max_fdist >= -cEpsilon) && (eyes_max_fdist <= cEpsilon)))
	{
		if (pNode->posChild != -1)
			TraverseBsp(pNode->posChild, pEyes, tBox1, pAction, param);
		if (pNode->negChild != -1)
			TraverseBsp(pNode->negChild, pEyes, tBox2, pAction, param);
		return;
	}

	if (eyes_min_fdist > cEpsilon && eyes_max_fdist < cEpsilon)
	{
		if (pNode->posChild != -1)
			TraverseBsp(pNode->posChild, pEyes, tBox1, pAction, param);
		return;
	}

	if (eyes_min_fdist < -cEpsilon && eyes_max_fdist < -cEpsilon)
	{
		if (pNode->negChild != -1)
			TraverseBsp(pNode->negChild, pEyes, tBox2, pAction, param);
		return;
	}

	float eyesmin_div_deltadist = (float)(eyes_min_fdist / (eyes_min_fdist - eyes_max_fdist));
	AjustDelta(pEyes, (float*)&ajusted.x, eyesmin_div_deltadist);

	if (eyes_min_fdist <= 0.0)
	{
		if (pNode->negChild != -1)
		{
			MergeBox(newEyes, &pEyes[0][0], (float*)&ajusted.x);
			TraverseBsp(pNode->negChild, newEyes, tBox2, pAction, param);
		}
		if (pNode->posChild != -1)
		{
			MergeBox(newEyes, (float*)&ajusted.x, &pEyes[1][0]);
			TraverseBsp(pNode->posChild, newEyes, tBox1, pAction, param);
		}
	}
	else
	{
		if (pNode->posChild != (short)-1)
		{
			MergeBox(newEyes, &pEyes[0][0], (float*)&ajusted.x);
			TraverseBsp(pNode->posChild, newEyes, tBox1, pAction, param);
		}
		if (pNode->negChild != (short)-1)
		{
			MergeBox(newEyes, (float*)&ajusted.x, &pEyes[1][0]);
			TraverseBsp(pNode->negChild, newEyes, tBox2, pAction, param);
		}
	}
}