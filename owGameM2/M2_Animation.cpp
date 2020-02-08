#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Animation.h"

CM2_Animation::CM2_Animation(const M2& M2Model, uint16 _animID, std::string _name, uint16 indexIntoSeq, const SM2_Sequence& _sequence) :
	m_AnimID(_animID),
	m_Name(_name),
	m_SequenceIndex(indexIntoSeq),
	m_Next(nullptr)
{
	int16 nextIndex = _sequence.variationNext;
	if (nextIndex != -1)
	{
		m_Next = new CM2_Animation(M2Model, _animID, (_name + "" + std::to_string(nextIndex)), nextIndex, M2Model.m_Sequences[nextIndex]);
	}

	m_StartTimeStamp = _sequence.start_timestamp;
	m_EndTimeStamp = _sequence.end_timestamp;
}

CM2_Animation::~CM2_Animation()
{
	if (m_Next != nullptr)
	{
		delete m_Next;
		m_Next = nullptr;
	}
}
