#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Animator.h"

CM2_Animator::CM2_Animator(const std::weak_ptr<const M2> _m2) :
	m_M2(_m2),
	m_IsLoop(false),
	m_IsPlayed(false),
	//m_OnAnimationEnded(nullptr),
	animtime(0.0),
	m_CurrentTime(0)
{
	std::shared_ptr<const M2> M2 = m_M2.lock();
	assert1(M2 != nullptr);

	for (auto& i : DBC_AnimationData)
	{
		std::shared_ptr<DBC_AnimationDataRecord> record = i;

		// Get animation with index (record->Get_ID() and variation index == 0)
		int16 findedSeqIndex = -1;
		for (uint16 j = 0; j < M2->m_Sequences.size(); j++)
		{
			if (M2->m_Sequences[j].__animID == record->Get_ID() && M2->m_Sequences[j].variationIndex == 0)
			{
				findedSeqIndex = j;
				break;
			}
		}

		if (findedSeqIndex == -1) continue;


		std::shared_ptr<CM2_Animation> animation = std::make_shared<CM2_Animation>(M2, record->Get_ID(), record->Get_Name(), findedSeqIndex, M2->m_Sequences[findedSeqIndex]);
		m_Animations.insert(std::make_pair(record->Get_ID(), animation));
		//Log::Warn("Animation [%d] '%s'", record->Get_ID(), record->Get_Name());
	}

	assert1(m_Animations.size() > 0);
	PlayAnimation();
}

CM2_Animator::~CM2_Animator()
{
	//ERASE_MAP(m_Animations);
}

void CM2_Animator::PlayAnimation(int16 _id, bool _loop)
{
	m_IsLoop = _loop;

	if (_id == -1)
	{
		m_CurrentAnimation = m_Animations.begin()->second.operator->();
	}
	else
	{
		if ((m_Animations.find(_id) == m_Animations.end()))
		{
			Log::Error("ANIMATION [%d] not found!", _id);
			return;
		}

		m_CurrentAnimation = (m_Animations[_id].operator->());
	}

	m_CurrentTime = m_CurrentAnimation->getStart();
	m_IsPlayed = false;
	animtime = 0.0;
}

void CM2_Animator::PrintList()
{
	for (auto& it : m_Animations)
	{
		Log::Warn("[%d] is [%s]", it.first, it.second->getName().c_str());
	}
}

void CM2_Animator::Update(double _time, double _dTime)
{
	if (m_IsPlayed)
	{
		return;
	}

	animtime += (_dTime * 1.0);
	m_CurrentTime = static_cast<uint32>(m_CurrentAnimation->getStart() + animtime);

	// Animation don't ended
	if (m_CurrentTime < m_CurrentAnimation->getEnd())
	{
		return;
	}

	// Ended!
	/*const CM2_Animation* nextAnim = m_CurrentAnimation->getNext();
	if (m_CurrentAnimation->getNext() != nullptr)
	{
		m_CurrentAnimation = nextAnim;
		m_CurrentTime = m_CurrentAnimation->getStart();
		m_IsPlayed = false;
		animtime = 0;
		return;
	}*/

	m_CurrentTime = m_CurrentAnimation->getEnd() - 1;
	m_IsPlayed = true;

	/*if (m_OnAnimationEnded != nullptr)
	{
		m_OnAnimationEnded->operator()();
	}*/

	if (m_IsLoop)
	{
		PlayAnimation(m_CurrentAnimation->getAnimID());
	}
}

/*void CM2_Animator::setOnEndFunction(Function* _onEnd)
{
	if (m_OnAnimationEnded != nullptr)
	{
		delete m_OnAnimationEnded;
	}

	assert1(_onEnd);
	m_OnAnimationEnded = _onEnd;
}*/
