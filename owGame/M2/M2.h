#pragma once

#include "M2_Headers.h"

// COMPONENTS BEGIN
#include "M2_Animator.h"
#include "M2_Comp_Materials.h"
#include "M2_Comp_Miscellaneous.h"
#include "M2_Comp_Skeleton.h"
// COMPONETS END

// Skins
#include "M2_Skin.h"

// FORWARD BEGIN
class CM2_Builder;
class CM2_Skin_Builder;
// FORWARD END

class M2 
	: public ISceneNodeProvider
	, public CLoadableObject
{
	friend class CM2_Builder;
	friend class CM2_Skin_Builder;
public:
	M2(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const std::string& name);
	virtual ~M2();

	// ISceneNodeProvider
	void CreateInsances(ISceneNode3D* _parent) const override;

	// CLoadableObject
	bool Load() override;

	void update(double _time, double _dTime);
	void calc(uint16 _animationIndex, uint32 _time, uint32 globalTime, cmat4 _viewMatrix, cmat4 _worldMatrix);

public:
	std::string getFilename() const { return m_FileName; }
	std::string getFilenameWithoutExt() const { return m_FileNameWithoutExt; }
	std::string getFilePath() const { return m_FilePath; }
	std::string getUniqueName() const { return m_UniqueName; }
	cbbox GetBounds() const { return m_Bounds; }
	
#pragma region Loops & Sequences
public:
	const SM2_Sequence& getSequence(uint32 _index) const
	{
		_ASSERT(_index < m_SequencesLookup.size());
		int16 newIndex = m_SequencesLookup[_index];
		_ASSERT(newIndex < static_cast<int16>(m_Sequences.size()));
		return m_Sequences[newIndex];
	}
	const bool isAnimated() const { return m_IsAnimated; }
public:
	GlobalLoopSeq						m_GlobalLoops;
	std::vector<SM2_Sequence>				m_Sequences;
	std::vector<int16>						m_SequencesLookup;
	bool								m_IsAnimated;
#pragma endregion

public:
	std::shared_ptr<CM2_Comp_Materials> getMaterials() const { return m_Materials; }
	std::shared_ptr<CM2_Comp_Materials> m_Materials;

	std::shared_ptr<CM2_Comp_Miscellaneous> getMiscellaneous() const { return m_Miscellaneous; }
	std::shared_ptr<CM2_Comp_Miscellaneous> m_Miscellaneous;

	std::shared_ptr<CM2_Comp_Skeleton> getSkeleton() const { return m_Skeleton; }
	std::shared_ptr<CM2_Comp_Skeleton> m_Skeleton;

private:
	std::string							m_FileName;
	std::string							m_FileNameWithoutExt;
	std::string							m_FilePath;

	std::string							m_UniqueName;
	BoundingBox							m_Bounds;

	// Vertices
	bool								m_IsContainGeom;

	// Skins
	std::vector<std::shared_ptr<CM2_Skin>>				m_Skins;

private:
	// Buffers and geom
	std::shared_ptr<IModel>				m_CollisionGeom;
	uint32								m_CollisionIndCnt;
	uint32								m_CollisionVetCnt;

private:
	IBaseManager* m_BaseManager;
	IRenderDevice& m_RenderDevice;

private: // Static and Consts
	const uint8							C_TexturesMaxCount = 128;
	const uint8							C_BonesInfluences = 4;
};
