#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Animation.h"

CM2_Animation::CM2_Animation(const M2& M2Model, const SM2_Sequence& Sequence, std::string AnimationName, uint16 IndexIntoSeq)
	: m_AnimID(Sequence.__animID)
	, m_AnimationName(AnimationName + "_" + std::to_string(IndexIntoSeq))
	, m_SequenceIndex(IndexIntoSeq)
	, m_StartTimeStamp(Sequence.start_timestamp)
    , m_EndTimeStamp(Sequence.end_timestamp)
{
	if (Sequence.variationNext != -1)
	{
		_ASSERT(Sequence.variationNext >= 0 && Sequence.variationNext < M2Model.m_Sequences.size());
		const SM2_Sequence& variationNextSequence = M2Model.m_Sequences[Sequence.variationNext];

		_ASSERT(variationNextSequence.__animID == m_AnimID);
		//_ASSERT(variationNextSequence.variationIndex == Sequence.variationNext);
		m_Next = std::make_unique<CM2_Animation>(M2Model, variationNextSequence, AnimationName, Sequence.variationNext);
	}
}

CM2_Animation::~CM2_Animation()
{

}
