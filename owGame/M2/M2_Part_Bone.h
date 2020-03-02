#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class M2;
class CM2_Comp_Skeleton;
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

class CM2_Part_Bone
	: public std::enable_shared_from_this<CM2_Part_Bone>
{
public:
	CM2_Part_Bone(const M2& M2Object, const std::shared_ptr<IFile>& File, int16 boneDirectIndex, const SM2_Bone& M2Bone);
	virtual ~CM2_Part_Bone();

	glm::mat4 calcMatrix(uint16 anim, uint32 time, uint32 globalTime) const;
	glm::mat4 calcBillboardMatrix(cmat4 _viewMatrix, cmat4 _worldMatrix) const;

	bool IsInterpolated(uint16 anim) const
	{
		return m_TranslateAnimated.IsUsesBySequence(anim) || 
			m_RotateAnimated.IsUsesBySequence(anim)       || 
			m_ScaleAnimated.IsUsesBySequence(anim);
	}

	bool IsBillboard() const
	{
		return m_Flags.spherical_billboard       ||
			m_Flags.cylindrical_billboard_lock_x ||
			m_Flags.cylindrical_billboard_lock_y ||
			m_Flags.cylindrical_billboard_lock_z;
	}

	int32                               getGameBoneID() const { return m_GameBoneId; }
	int16                               getParentBoneID_DirectArray() const { return m_ParentBoneID; }
	std::weak_ptr<const CM2_Part_Bone>  getParentBone() const { return m_ParentBone; }
	const std::vector<std::shared_ptr<const CM2_Part_Bone>>& getChildBones() const { return m_ChildBones; }
	uint16                              getSubmeshID() const { return m_submesh; }
	const glm::vec3&                    getPivot() const { return m_PivotPoint; }

	void                                Print(size_t intent) const;

	glm::mat4                           getTransformMatrix() const { return glm::mat4(1.0); }
	glm::mat4                           getRotateMatrix() const { return glm::mat4(1.0); }


public: // Call this after initialization
	void SetParentBoneInternal(const CM2_Comp_Skeleton& Skeleton);


private:
	std::string                         m_GameBoneName;
	int32                               m_GameBoneId; // Bones lookup table
	int16                               m_DirectIndex;
	SM2_Bone::Flags                     m_Flags;
	int16							    m_ParentBoneID;
	std::weak_ptr<const CM2_Part_Bone>  m_ParentBone;
	std::vector<std::shared_ptr<const CM2_Part_Bone>> m_ChildBones;
	uint16							    m_submesh;
	glm::vec3                           m_PivotPoint;
	M2_Animated<vec3>                   m_TranslateAnimated;
#if defined(WOW_CLASSIC_1_12_1)
	M2_Animated<quat> m_RotateAnimated;
#elif defined(WOW_BC_2_4_3)
	M2_Animated<quat, M2CompQuat, Quat16ToQuat32> m_RotateAnimated;
#endif
	M2_Animated<vec3>                   m_ScaleAnimated;

private:
	const M2& m_M2Object;
};


