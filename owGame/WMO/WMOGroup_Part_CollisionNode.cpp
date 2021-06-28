#include "stdafx.h"

#ifdef USE_WMO_MODELS

// Include 
#include "WMOGroup.h"

// General
#include "WMOGroup_Part_CollisionNode.h"

CWMOGroup_Part_CollisionNode::CWMOGroup_Part_CollisionNode(const CWMOGroup& WMOGroup, const SWMOGroup_MOBN& _proto) :
	m_Proto(_proto)
{
	std::vector<glm::vec3> collisionVertices;
	collisionVertices.reserve(m_Proto.nFaces * 3);
	/*for (uint32 i = m_Proto.faceStart; i < m_Proto.faceStart + m_Proto.nFaces; i++)
	{
		collisionVertices.push_back(_parentGroup->dataFromMOVT[_parentGroup->collisionIndexes[3 * i + 0]]);
		collisionVertices.push_back(_parentGroup->dataFromMOVT[_parentGroup->collisionIndexes[3 * i + 1]]);
		collisionVertices.push_back(_parentGroup->dataFromMOVT[_parentGroup->collisionIndexes[3 * i + 2]]);
	}*/

	/*std::shared_ptr<IBuffer> vb = _Render->r.createVertexBuffer(collisionVertices.size() * sizeof(vec3), collisionVertices.data(), false);

	m_GEOM_Collision = _Render->r.beginCreatingGeometry(PRIM_TRILIST, _Render->getRenderStorage()->__layout_GxVBF_P);
	m_GEOM_Collision->setGeomVertexParams(vb, R_DataType::T_FLOAT, 0, sizeof(vec3));
	m_GEOM_Collision->finishCreatingGeometry();*/

	color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
}

CWMOGroup_Part_CollisionNode::~CWMOGroup_Part_CollisionNode()
{
}

void CWMOGroup_Part_CollisionNode::Render(const glm::mat4& _worldMatrix)
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

#endif
