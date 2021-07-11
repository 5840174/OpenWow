#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_AnimatorComponent.h"

CM2AnimatorComponent::CM2AnimatorComponent(const CM2_Base_Instance& OwnerNode)
	: CSceneNodeComponentBase(OwnerNode)
	
	, m_CurrentAnimationID(EAnimationID::Stand)
	, m_CurrentAnimation(nullptr)
	
	, m_IsLoop(false)
	, m_IsStopped(false)

	, m_AnimTime(0.0)
	, m_CurrentTime(0)
{

}

CM2AnimatorComponent::~CM2AnimatorComponent()
{
}

void CM2AnimatorComponent::LoadAnimations()
{
	const auto& sequences = GetM2OwnerNode().GetM2().getSkeleton().GetSequences();
	for (uint16 j = 0; j < sequences.size(); j++)
	{
		const auto& sequence = sequences[j];
		if (sequence.variationIndex == 0)
		{
			const DBC_AnimationDataRecord* dbcAnimationRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_AnimationData()[sequence.__animID];
			if (dbcAnimationRecord == nullptr)
				throw CException("CM2AnimatorComponent::CM2AnimatorComponent: Sequence '%d' not found in 'DBC_AnimationData'.", sequence.__animID);

			m_Animations.insert(std::make_pair(sequence.__animID, MakeShared(CM2_Animation, GetM2OwnerNode().GetM2(), sequence, dbcAnimationRecord->Get_Name(), j)));
		}
	}

	_ASSERT(m_Animations.size() > 0);
	PlayAnimation(m_CurrentAnimationID, true);
}

void CM2AnimatorComponent::PlayAnimation(EAnimationID AnimationId, bool Loop)
{
	const auto& animIt = m_Animations.find((uint16)AnimationId);
	if (animIt != m_Animations.end())
	{
		m_CurrentAnimationID = AnimationId;
		m_CurrentAnimation = animIt->second.get();
	}
	else
	{
		m_CurrentAnimationID = EAnimationID::Stand;
		m_CurrentAnimation = m_Animations.begin()->second.get();
		//Log::Error("CM2AnimatorComponent: Animation '%d' not found. Playing first animation '%s' ('%d').", AnimationId, m_CurrentAnimation->getAnimationName().c_str(), m_CurrentAnimation->getAnimID());
	}

	m_IsLoop = Loop;
	m_IsStopped = false;
	m_AnimTime = 0.0;
	m_CurrentTime = m_CurrentAnimation->getStart();
}

void CM2AnimatorComponent::PrintList()
{
	for (auto& it : m_Animations)
	{
		Log::Warn("[%d] is [%s]", it.first, it.second->getAnimationName().c_str());
	}
}

void CM2AnimatorComponent::Update(const UpdateEventArgs & e)
{
	if (m_CurrentAnimation == nullptr)
		return;

	if (m_IsStopped)
		return;

	m_AnimTime += e.DeltaTime;
	m_CurrentTime = static_cast<uint32>(m_CurrentAnimation->getStart() + m_AnimTime);

	// Animation don't ended
	if (m_CurrentTime < m_CurrentAnimation->getEnd())
		return;

	// Ended!
	if (m_CurrentAnimation->hasNextVatianton())
	{
		m_CurrentAnimation = m_CurrentAnimation->getNextVariation();
		m_CurrentTime = m_CurrentAnimation->getStart();
		m_IsStopped = false;
		m_AnimTime = 0;
		return;
	}

	m_CurrentTime = m_CurrentAnimation->getEnd() - 1;
	m_IsStopped = true;

	if (m_IsLoop)
	{
		PlayAnimation(m_CurrentAnimation->getAnimID(), true);
	}
}



//
// Protected
//
const CM2_Base_Instance & CM2AnimatorComponent::GetM2OwnerNode() const
{
	return dynamic_cast<const CM2_Base_Instance&>(GetOwnerNode());
}
