#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Animator.h"

CM2_Animator::CM2_Animator(const IBaseManager& BaseManager, const M2& M2Model) 
	: m_M2Model(M2Model)
	, m_IsLoop(false)
	, m_IsStopped(false)
	, animtime(0.0)
	, m_CurrentTime(0)
{
	for (uint16 j = 0; j < m_M2Model.getSkeleton().getSequencesCount(); j++)
	{
		const SM2_Sequence& sequence = m_M2Model.getSkeleton().getSequenceDirect(j);

		if (sequence.variationIndex == 0)
		{
			const DBC_AnimationDataRecord* dbcAnimationRecord = BaseManager.GetManager<CDBCStorage>()->DBC_AnimationData()[sequence.__animID];
			_ASSERT(dbcAnimationRecord != nullptr);

			m_Animations.insert(std::make_pair(sequence.__animID, std::make_shared<CM2_Animation>(m_M2Model, sequence, dbcAnimationRecord->Get_Name(), j)));
		}
	}

	_ASSERT(m_Animations.size() > 0);
	PlayAnimation(0, true);
}

CM2_Animator::~CM2_Animator()
{
	//ERASE_MAP(m_Animations);
}

void CM2_Animator::PlayAnimation(uint16 AnimationId, bool Loop)
{
	m_IsLoop = Loop;

	const auto& animIt = m_Animations.find(AnimationId);
	if (animIt != m_Animations.end())
	{
		m_CurrentAnimation = animIt->second.get();
	}
	else
	{
		m_CurrentAnimation = m_Animations.begin()->second.get();
		//Log::Error("CM2_Animator: Animation '%d' not found. Playing first animation '%s' ('%d').", AnimationId, m_CurrentAnimation->getAnimationName().c_str(), m_CurrentAnimation->getAnimID());
	}

	m_CurrentTime = m_CurrentAnimation->getStart();
	m_IsStopped = false;
	animtime = 0.0;
}

void CM2_Animator::PrintList()
{
	for (auto& it : m_Animations)
	{
		Log::Warn("[%d] is [%s]", it.first, it.second->getAnimationName().c_str());
	}
}

void CM2_Animator::Update(double _time, double _dTime)
{
	if (m_IsStopped)
		return;

	animtime += (_dTime * 1.0);
	m_CurrentTime = static_cast<uint32>(m_CurrentAnimation->getStart() + animtime);

	// Animation don't ended
	if (m_CurrentTime < m_CurrentAnimation->getEnd())
		return;

	// Ended!
	/*if (m_CurrentAnimation->hasNextVatianton())
	{
		m_CurrentAnimation = m_CurrentAnimation->getNextVariation();
		m_CurrentTime = m_CurrentAnimation->getStart();
		m_IsPlayed = false;
		animtime = 0;
		return;
	}*/

	m_CurrentTime = m_CurrentAnimation->getEnd() - 1;
	m_IsStopped = true;

	if (m_IsLoop)
	{
		PlayAnimation(m_CurrentAnimation->getAnimID(), true);
	}
}
