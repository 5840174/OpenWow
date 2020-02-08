#include "stdafx.h"

// General
#include "M2_Comp_Skeleton.h"

CM2_Comp_Skeleton::CM2_Comp_Skeleton()	:
	m_HasBones(false),
	m_IsAnimBones(false),
	m_IsBillboard(false)
{
}

void CM2_Comp_Skeleton::calc(uint16 anim, uint32 time, uint32 globalTime, cmat4 _viewMat, cmat4 _worldMat)
{
	// Bones
	if (m_HasBones)
	{
		for (uint32 i = 0; i < m_Bones.size(); i++)
		{
			m_Bones[i]->SetNeedCalculate();
		}

		for (uint32 i = 0; i < m_Bones.size(); i++)
		{
			m_Bones[i]->calcMatrix(anim, time, globalTime);
		}

		for (uint32 i = 0; i < m_Bones.size(); i++)
		{
			m_Bones[i]->calcBillboard(_viewMat, _worldMat);
		}
	}
}
