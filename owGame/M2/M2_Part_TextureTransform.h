#pragma once

#include "M2_Types.h"

class CM2_Part_TextureTransform
{
public:
	CM2_Part_TextureTransform(std::shared_ptr<IFile> f, const SM2_TextureTransform& _proto, cGlobalLoopSeq global);

	void calc(uint16 anim, uint32 time, uint32 globalTime);

	cmat4 getValue() const { return matrix; }

private:
	M2_Animated<glm::vec3> trans;
	M2_Animated<glm::quat> roll;
	M2_Animated<glm::vec3> scale;

	glm::mat4					matrix;
};