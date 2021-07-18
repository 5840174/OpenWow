#include "stdafx.h"

#ifdef USE_WMO_MODELS

// Include 
#include "WMOGroup.h"

// General
#include "WMOGroup_Part_CollisionNode.h"

CWMOGroup_Part_CollisionNode::CWMOGroup_Part_CollisionNode(IRenderDevice& RenderDevice, const CWMOGroup& WMOGroup, const SWMOGroup_MOBN& Proto, const std::vector<glm::vec3>& VerticesArray, const std::vector<uint16>& CollisionIndicesArray)
	: m_Proto(Proto)
{


	if (m_Proto.nFaces == 0)
		return;

	std::vector<glm::vec3> collisionVerticesArray;
	collisionVerticesArray.reserve(m_Proto.nFaces * 3);
	for (uint32 i = m_Proto.faceStart; i < m_Proto.faceStart + m_Proto.nFaces; i++)
	{
		collisionVerticesArray.push_back(VerticesArray[CollisionIndicesArray[3 * i + 0]]);
		collisionVerticesArray.push_back(VerticesArray[CollisionIndicesArray[3 * i + 1]]);
		collisionVerticesArray.push_back(VerticesArray[CollisionIndicesArray[3 * i + 2]]);
	}
	//for (uint32 i = m_Proto.faceStart; i < m_Proto.faceStart + m_Proto.nFaces; i++)
	//{
	//	for (uint8 j = 0; j < 3; j++)
	//	{
	//		const size_t index = ((i - m_Proto.faceStart) * 3) + j;
	//		const size_t index2 = ((i - m_Proto.faceStart) * 3) + j + m_Proto.faceStart;
	//
	//		collisionVerticesArray[index] = VerticesArray[CollisionIndicesArray[index2]];
	//	}
	//}

	if ((m_Proto.flags & SWMOGroup_MOBN::Flag_Leaf))
	{
		auto collisitonVertexBuffer = RenderDevice.GetObjectsFactory().CreateVertexBuffer(collisionVerticesArray);

		m_CollisionGeom = RenderDevice.GetObjectsFactory().CreateGeometry();
		m_CollisionGeom->SetVertexBuffer(collisitonVertexBuffer);
	}
}

CWMOGroup_Part_CollisionNode::~CWMOGroup_Part_CollisionNode()
{
}

const std::shared_ptr<IGeometry>& CWMOGroup_Part_CollisionNode::GetCollisionGeometry() const
{
	return m_CollisionGeom;
}

#endif
