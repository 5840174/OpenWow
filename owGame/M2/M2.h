#pragma once

#include "M2_Headers.h"

// Components
#include "M2_Animator.h"
#include "M2_Comp_Materials.h"
#include "M2_Comp_Miscellaneous.h"
#include "M2_Comp_Skeleton.h"

// Skins
#include "M2_Skin.h"

class M2 
	: public ISceneNodeProvider
	, public CLoadableObject
{
public:
	M2(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::string& FileName);
	virtual ~M2();

	// ISceneNodeProvider
	void CreateInsances(const std::shared_ptr<ISceneNode3D>& Parent) const override;

	// CLoadableObject
	bool Load() override;

	void update(double _time, double _dTime);

	IBaseManager& GetBaseManager() const { return m_BaseManager; }
	IRenderDevice& GetRenderDevice() const { return m_RenderDevice; }

public:
	std::string getFilename() const { return m_FileName; }
	std::string getFilenameWithoutExt() const { return m_FileNameWithoutExt; }
	std::string getFilePath() const { return m_FilePath; }
	std::string getUniqueName() const { return m_UniqueName; }
	cbbox       GetBounds() const { return m_Bounds; }
	

public: // Loops & Sequences
	const std::vector<SM2_Loop>& getGlobalLoops() const
	{
		return m_GlobalLoops;
	}
	const SM2_Sequence& getSequence(uint32 _index) const
	{
		_ASSERT(_index < m_SequencesLookup.size());
		int16 newIndex = m_SequencesLookup[_index];
		_ASSERT(newIndex < static_cast<int16>(m_Sequences.size()));
		return m_Sequences[newIndex];
	}
	const bool isAnimated() const { return m_IsAnimated; }
public:
	std::vector<SM2_Loop>      m_GlobalLoops;
	std::vector<SM2_Sequence>  m_Sequences;
	std::vector<int16>         m_SequencesLookup;
	bool                       m_IsAnimated;


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
	std::shared_ptr<IFile>              m_F;

private:
	SM2_Header				            m_Header;
	std::string							m_UniqueName;
	BoundingBox							m_Bounds;

	// Skins
	std::vector<std::shared_ptr<CM2_Skin>> m_Skins;

private:
	// Buffers and geom
	std::shared_ptr<IModel>				m_CollisionGeom;
	uint32								m_CollisionIndCnt;
	uint32								m_CollisionVetCnt;

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
};
