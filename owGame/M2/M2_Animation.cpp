#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "M2.h"

// General
#include "M2_Animation.h"

CM2_Animation::CM2_Animation(const CM2& M2Model, const SM2_Sequence& Sequence, std::string AnimationName, uint16 IndexIntoSeq)
	: m_AnimID(Sequence.__animID)
	, m_AnimationName(AnimationName + "_" + std::to_string(IndexIntoSeq))
	, m_SequenceIndex(IndexIntoSeq)
#if WOW_CLIENT_VERSION <= WOW_BC_2_4_3
	, m_StartTimeStamp(Sequence.start_timestamp)
    , m_EndTimeStamp(Sequence.end_timestamp)
#else
	, m_Duration(Sequence.duration)
#endif
{
	if (Sequence.variationNext != -1)
	{
		_ASSERT(Sequence.variationNext >= 0 && Sequence.variationNext < M2Model.getSkeleton().GetSequences().size());
		const SM2_Sequence& variationNextSequence = M2Model.getSkeleton().GetSequences()[Sequence.variationNext];

		_ASSERT(variationNextSequence.__animID == m_AnimID);
		//_ASSERT(variationNextSequence.variationIndex == Sequence.variationNext);
		m_Next = std::make_unique<CM2_Animation>(M2Model, variationNextSequence, AnimationName, Sequence.variationNext);
	}
}

CM2_Animation::~CM2_Animation()
{

}

#endif
