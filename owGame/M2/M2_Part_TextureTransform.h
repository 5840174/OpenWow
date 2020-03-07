#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

class CM2_Part_TextureTransform
{
public:
	CM2_Part_TextureTransform(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_TextureTransform& M2TextureTransform);
	virtual ~CM2_Part_TextureTransform();

	glm::mat4 GetTransform(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const;

private:
	M2_Animated<glm::vec3> m_TranslateAnimated;
	M2_Animated<glm::quat> m_RotateAnimated;
	M2_Animated<glm::vec3> m_ScaleAnimated;

	const CM2& m_M2Object;
};