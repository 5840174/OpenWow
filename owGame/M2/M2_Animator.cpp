#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Animator.h"

CM2_Animator::CM2_Animator(const IBaseManager* BaseManager, const M2& M2Model) :
	m_M2Model(M2Model),
	m_IsLoop(false),
	m_IsPlayed(false),
	//m_OnAnimationEnded(nullptr),
	animtime(0.0),
	m_CurrentTime(0)
{
	for (const auto& i : BaseManager->GetManager<CDBCStorage>()->DBC_AnimationData())
	{
		std::shared_ptr<DBC_AnimationDataRecord> record = i;

		// Get animation with index (record->Get_ID() and variation index == 0)
		int16 findedSeqIndex = -1;
		for (uint16 j = 0; j < m_M2Model.m_Sequences.size(); j++)
		{
			if (m_M2Model.m_Sequences[j].__animID == record->Get_ID() && m_M2Model.m_Sequences[j].variationIndex == 0)
			{
				findedSeqIndex = j;
				break;
			}
		}

		if (findedSeqIndex == -1) continue;


		std::shared_ptr<CM2_Animation> animation = std::make_shared<CM2_Animation>(m_M2Model, record->Get_ID(), record->Get_Name(), findedSeqIndex, m_M2Model.m_Sequences[findedSeqIndex]);
		m_Animations.insert(std::make_pair(record->Get_ID(), animation));
		//Log::Warn("Animation [%d] '%s'", record->Get_ID(), record->Get_Name());
	}

	_ASSERT(m_Animations.size() > 0);
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

	_ASSERT(_onEnd);
	m_OnAnimationEnded = _onEnd;
}*/
