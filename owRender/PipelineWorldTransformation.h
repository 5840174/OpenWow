#pragma once

class PipelineWorldTransformation
{
public:
	PipelineWorldTransformation() 
	{ 
		Clear();
	}

	inline void Clear()
	{
		worldTransformation = mat4();
	}

	inline void Translate(float x, float y, float z);
	inline void Translate(cvec3 _translate);

	inline void Rotate(float _yaw, float _pitch, float _roll);
	inline void Rotate(cvec3 _rotation);

	inline void Scale(float _scale);
	inline void Scale(float _scaleX, float _scaleY, float _scaleZ);
	inline void Scale(cvec3 _scale);

	//

	inline void SetWorld(const mat4 _world);
	inline const mat4& GetWorld();

private:
	mat4 worldTransformation;
};

#include "PipelineWorldTransformation.inl"