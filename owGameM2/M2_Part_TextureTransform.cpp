#include "stdafx.h"

// General
#include "M2_Part_TextureTransform.h"

CM2_Part_TextureTransform::CM2_Part_TextureTransform(std::shared_ptr<IFile> f, const SM2_TextureTransform& _proto, cGlobalLoopSeq global)
{
	trans.init(_proto.translation, f, global);
	roll.init(_proto.rotation, f, global);
	scale.init(_proto.scaling, f, global);
}

void CM2_Part_TextureTransform::calc(uint16 anim, uint32 time, uint32 globalTime)
{
	matrix = mat4(1.0f);
	
	vec3 transValue;
	if (trans.uses(anim))
	{
		transValue = trans.getValue(anim, time, globalTime);
		matrix = glm::translate(matrix, transValue);
	}

	quat rollValue;
	if (roll.uses(anim))
	{
		matrix = glm::translate(matrix, glm::vec3(0.5f, 0.5f, 0.5f));

		rollValue = roll.getValue(anim, time, globalTime);
		matrix *= glm::toMat4(rollValue);

		matrix = glm::translate(matrix, vec3(-0.5f, -0.5f, -0.5f));
	}

	vec3 scaleVal;
	if (scale.uses(anim))
	{
		scaleVal = scale.getValue(anim, time, globalTime);
		matrix = glm::scale(matrix, scaleVal);
	}
}

