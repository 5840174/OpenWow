#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"
#include "M2_Skin_Builder.h"

// General
#include "M2_Skin.h"

// Additional
#include "M2_Part_Material.h"

CM2_Skin::CM2_Skin(const std::weak_ptr<const M2> _model) 
	: m_ParentM2(_model)
{}

void CM2_Skin::CreateInsances(std::weak_ptr<ISceneNode> _parent)
{
	for (const auto& batch : m_Batches)
	{
		_parent.lock()->GetComponent<IMeshComponent3D>()->AddMesh(batch);
	}
}