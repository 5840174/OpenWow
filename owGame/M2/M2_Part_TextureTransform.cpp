#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_TextureTransform.h"

CM2_Part_TextureTransform::CM2_Part_TextureTransform(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_TextureTransform& M2TextureTransform)
	: m_M2Object(M2Object)
{
	m_TranslateAnimated.Initialize(M2TextureTransform.translation, File);
	m_RotateAnimated.Initialize(M2TextureTransform.rotation, File);
	m_ScaleAnimated.Initialize(M2TextureTransform.scaling, File);
}

CM2_Part_TextureTransform::~CM2_Part_TextureTransform()
{
}

glm::mat4 CM2_Part_TextureTransform::GetTransform(uint16 Sequence, uint32 Time, uint32 GlobalTime) const
{
	glm::mat4 matrix = glm::mat4(1.0f);

	if (m_TranslateAnimated.IsUsesBySequence(Sequence))
		matrix = glm::translate(matrix, m_TranslateAnimated.GetValue(Sequence, Time, m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime));

	if (m_RotateAnimated.IsUsesBySequence(Sequence))
	{
		matrix = glm::translate(matrix, glm::vec3( 0.5f));
		matrix *= glm::toMat4(m_RotateAnimated.GetValue(Sequence, Time, m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime));
		matrix = glm::translate(matrix, glm::vec3(-0.5f));
	}

	if (m_ScaleAnimated.IsUsesBySequence(Sequence))
		matrix = glm::scale(matrix, m_ScaleAnimated.GetValue(Sequence, Time, m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime));

	return matrix;
}
