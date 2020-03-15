#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

enum class ZN_API M2_GameBoneType : int16
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
	Root = 26
};

const char* const M2_GameBoneTypeNames[] = 
{
	"ArmL",
	"ArmR",

	"ShoulderL",
	"ShoulderR",

	"SpineLow",
	"Waist",

	"Head",
	"Jaw",

	"IndexFingerR",
	"MiddleFingerR",
	"PinkyFingerR",
	"RingFingerR",
	"ThumbR",

	"IndexFingerL",
	"MiddleFingerL",
	"PinkyFingerL",
	"RingFingerL",
	"ThumbL",

	"$BTH",
	"$CSR",
	"$CSL",
	"_Breath",
	"_Name",
	"_NameMount",
	"$CHD",
	"$CCH",
	"Root"
};

class SM2_Part_Bone_Wrapper
{
public:
	SM2_Part_Bone_Wrapper(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Bone& M2Bone);
	virtual ~SM2_Part_Bone_Wrapper();

	glm::mat4 calcMatrix(const CM2_Base_Instance* M2Instance, uint32 globalTime) const;
	glm::mat4 calcBillboardMatrix(const glm::mat4& _viewMatrix, const glm::mat4& _worldMatrix) const;

	bool IsInterpolated(uint16 anim) const
	{
		return m_TranslateAnimated.IsUsesBySequence(anim) || 
			   m_RotateAnimated.IsUsesBySequence(anim)    || 
			   m_ScaleAnimated.IsUsesBySequence(anim);
	}

	bool IsBillboard() const
	{
		return m_M2Bone.flags.spherical_billboard          ||
			   m_M2Bone.flags.cylindrical_billboard_lock_x ||
			   m_M2Bone.flags.cylindrical_billboard_lock_y ||
			   m_M2Bone.flags.cylindrical_billboard_lock_z;
	}

	const char*                         GetGameBoneName() const { return (m_M2Bone.key_bone_id != -1) ? M2_GameBoneTypeNames[m_M2Bone.key_bone_id] : "???"; }
	int32                               getGameBoneID() const { return m_M2Bone.key_bone_id; }
	int16                               getParentBoneID() const { return m_M2Bone.parent_bone; }
	uint16                              getSubmeshID() const { return m_M2Bone.submesh_id; }
	glm::vec3                           getPivot() const { return Fix_XZmY(m_M2Bone.pivot); }

private:
	M2_Animated<glm::vec3>              m_TranslateAnimated;
#if WOW_CLIENT_VERSION == WOW_CLASSIC_1_12_1
	M2_Animated<glm::quat>              m_RotateAnimated;
#elif WOW_CLIENT_VERSION == WOW_BC_2_4_3
	M2_Animated<glm::quat, M2CompQuat, Quat16ToQuat32> m_RotateAnimated;
#endif
	M2_Animated<glm::vec3>              m_ScaleAnimated;

private:
	const CM2& m_M2Object;
	const SM2_Bone m_M2Bone;
};
