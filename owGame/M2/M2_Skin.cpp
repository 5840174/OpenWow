#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"
#include "M2_Skin_Builder.h"

// General
#include "M2_Skin.h"

// Additional
#include "M2_Part_Material.h"

CM2_Skin::CM2_Skin(const M2& M2Model)
	: m_M2Model(M2Model)
{}

void CM2_Skin::CreateInsances(ISceneNode3D* _parent)
{
	for (const auto& batch : m_Batches)
	{
		_parent->GetComponent<IMeshComponent3D>()->AddMesh(batch);
	}
}