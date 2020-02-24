#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class M2;
class CM2_Comp_Skeleton;
// FORWARD END

struct M2_GameBoneType
{
	enum List : int16
	{
		ArmL = 0,
		ArmR = 1,

		ShoulderL = 2,
		ShoulderR = 3,

		SpineLow = 4,
		Waist = 5,

		Head = 6,
		Jaw = 7,

		IndexFingerR = 8,
		MiddleFingerR = 9,
		PinkyFingerR = 10,
		RingFingerR = 11,
		ThumbR = 12,

		IndexFingerL = 13,
		MiddleFingerL = 14,
		PinkyFingerL = 15,
		RingFingerL = 16,
		ThumbL = 17,

		$BTH = 18,
		$CSR = 19,
		$CSL = 20,
		_Breath = 21,
		_Name = 22,
		_NameMount = 23,
		$CHD = 24,
		$CCH = 25,
		Root = 26,

		Wheel1 = 27,
		Wheel2 = 28,
		Wheel3 = 29,
		Wheel4 = 30,
		Wheel5 = 31,
		Wheel6 = 32,
		Wheel7 = 33,
		Wheel8 = 34,

		Count
	};
};

class CM2_Part_Bone
{
public:
	CM2_Part_Bone(const M2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Bone& M2Bone);
	virtual ~CM2_Part_Bone();

	uint16 getSubmesh() const { return submesh; };

	void calcMatrix(uint16 anim, uint32 time, uint32 globalTime);
	void calcBillboard(cmat4 _viewMatrix, cmat4 _worldMatrix);

	bool IsInterpolated(uint16 anim) const
	{
		return trans.IsUsesBySequence(anim) || roll.IsUsesBySequence(anim) || scale.IsUsesBySequence(anim);
	}
	bool IsBillboard() const
	{
		return
			m_Flags.spherical_billboard ||
			m_Flags.cylindrical_billboard_lock_x ||
			m_Flags.cylindrical_billboard_lock_y ||
			m_Flags.cylindrical_billboard_lock_z;
	}

	void SetNeedCalculate() { m_IsCalculated = false; }
	bool IsCalculated() const { return m_IsCalculated; }

	int32			getGameBoneID() const { return m_GameBoneId; }
	std::weak_ptr<CM2_Part_Bone>	getParentBone() const { return m_ParentBone; }
	int16			getParentBoneID() const { return m_ParentBoneID; }
	cmat4			getTransformMatrix() const { return m_TransformMatrix; }
	cmat4			getRotateMatrix() const { return m_RotationMatrix; }
	cvec3			getPivot() const { return pivot; }
	cvec3			getTransPivot() const { return transPivot; }


public: // Call this after initialization
	void SetParentBoneInternal(const CM2_Comp_Skeleton& Skeleton);


private:
	int32           m_GameBoneId; // Bones lookup table
	SM2_Bone::Flags m_Flags;

	int16							m_ParentBoneID;
	std::weak_ptr<CM2_Part_Bone>	m_ParentBone;
	uint16							submesh;

	vec3 pivot, transPivot;

	mat4 m_TransformMatrix;
	mat4 m_RotationMatrix;

private:
	bool m_IsCalculated;

	M2_Animated<vec3> trans;
	M2_Animated<quat> roll;
	M2_Animated<vec3> scale;

private:
	const M2& m_M2Object;
};


