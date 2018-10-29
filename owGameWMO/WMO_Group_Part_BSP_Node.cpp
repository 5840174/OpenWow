#include "stdafx.h"

// Include 
#include "WMO_Group.h"

// General
#include "WMO_Group_Part_BSP_Node.h"

CWMO_Group_Part_BSP_Node::CWMO_Group_Part_BSP_Node(const std::weak_ptr<const WMO_Group> _parentGroup, const SWMO_Group_MOBNDef & _proto) :
	m_ParentGroup(_parentGroup)
{
	m_Proto = _proto;

	std::vector<vec3> collisionVertices;
	collisionVertices.reserve(m_Proto.nFaces * 3);
	/*for (uint32 i = m_Proto.faceStart; i < m_Proto.faceStart + m_Proto.nFaces; i++)
	{
		collisionVertices.push_back(_parentGroup->dataFromMOVT[_parentGroup->collisionIndexes[3 * i + 0]]);
		collisionVertices.push_back(_parentGroup->dataFromMOVT[_parentGroup->collisionIndexes[3 * i + 1]]);
		collisionVertices.push_back(_parentGroup->dataFromMOVT[_parentGroup->collisionIndexes[3 * i + 2]]);
	}*/

	/*std::shared_ptr<Buffer> vb = _Render->r.createVertexBuffer(collisionVertices.size() * sizeof(vec3), collisionVertices.data(), false);

	m_GEOM_Collision = _Render->r.beginCreatingGeometry(PRIM_TRILIST, _Render->getRenderStorage()->__layout_GxVBF_P);
	m_GEOM_Collision->setGeomVertexParams(vb, R_DataType::T_FLOAT, 0, sizeof(vec3));
	m_GEOM_Collision->finishCreatingGeometry();*/

	color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}

void CWMO_Group_Part_BSP_Node::Render(cmat4 _worldMatrix)
{
	//_Render->r.setFillMode(R_FillMode::RS_FILL_WIREFRAME);
	/*_Render->r.setCullMode(R_CullMode::RS_CULL_NONE);

	_Render->getTechniquesMgr()->Debug_Pass->Bind();
	{
		_Render->getTechniquesMgr()->Debug_Pass->setWorld(_worldMatrix);
		_Render->getTechniquesMgr()->Debug_Pass->SetColor4(color);

		_Render->r.setGeometry(m_GEOM_Collision);
		_Render->r.draw(0, m_Proto.nFaces);
	}
	_Render->getTechniquesMgr()->Debug_Pass->Unbind();

	_Render->r.setFillMode(R_FillMode::RS_FILL_SOLID);
	_Render->r.setCullMode(R_CullMode::RS_CULL_NONE);*/
}
