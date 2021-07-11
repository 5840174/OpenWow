#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Part_TextureTransform.h"

// C4Vector
// x - w
// y - x
// z - y
// w - z

glm::vec3 Fix_XZY2(const glm::vec3& _vec)
{
	return glm::vec3(_vec.x, _vec.y, _vec.z);
}

glm::quat Fix_XZmYW2(const glm::quat& _quat)
{
	return glm::quat(_quat.x, _quat.y, _quat.z, _quat.w);
}

CM2_Part_TextureTransform::CM2_Part_TextureTransform(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_TextureTransform& M2TextureTransform)
	: m_M2Object(M2Object)
{
	m_TranslateAnimated.Initialize(M2TextureTransform.translation, File, M2Object.getSkeleton().GetAnimFiles(), Fix_XZY2);
	m_RotateAnimated.Initialize(M2TextureTransform.rotation, File, M2Object.getSkeleton().GetAnimFiles(), Fix_XZmYW2);
	m_ScaleAnimated.Initialize(M2TextureTransform.scaling, File, M2Object.getSkeleton().GetAnimFiles());
}

CM2_Part_TextureTransform::~CM2_Part_TextureTransform()
{
}

glm::mat4 CM2_Part_TextureTransform::GetTransform(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const
{
	glm::mat4 matrix(1.0f);
	
	if (const auto& animator = M2Instance->GetAnimatorComponent())
	{
		if (m_TranslateAnimated.IsUsesBySequence(animator->getSequenceIndex()))
		{
			matrix = glm::translate(matrix, m_TranslateAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime));
		}

		if (m_RotateAnimated.IsUsesBySequence(animator->getSequenceIndex()))
		{
			matrix = glm::translate(matrix, glm::vec3( 0.5f,  0.5f, 0.0f)); // TODO: Maybe glm::vec2(0.5f, 0.5f, 0.0f) ?
			matrix *= glm::toMat4(m_RotateAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime));
			matrix = glm::translate(matrix, glm::vec3(-0.5f, -0.5f, 0.0f));
		}

		if (m_ScaleAnimated.IsUsesBySequence(animator->getSequenceIndex()))
		{
			matrix = glm::scale(matrix, m_ScaleAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime));
		}
	}
	

	return matrix;
}
