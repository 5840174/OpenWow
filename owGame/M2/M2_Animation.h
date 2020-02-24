#pragma once

#include "M2_Types.h"

class CM2_Animation
{
public:
	CM2_Animation(const M2& M2Model, const SM2_Sequence& Sequence, std::string AnimationName, uint16 IndexIntoSeq);
	virtual ~CM2_Animation();

	// Table data
	uint16					getAnimID()			const { return m_AnimID; }
	std::string				getAnimationName()	const { return m_AnimationName; }

	// Sequence
	uint16					getSequenceIndex()	const { return m_SequenceIndex; }
	bool                    hasNextVatianton()  const { return m_Next != nullptr; }
	const CM2_Animation*	getNextVariation()	const { return m_Next.get(); }

	// Times
	uint32					getStart()			const { return m_StartTimeStamp; }
	uint32					getEnd()			const { return m_EndTimeStamp; }

private:
	CM2_Animation(const CM2_Animation&) = delete;
	const CM2_Animation& operator=(CM2_Animation&) = delete;

private:
	const uint16			m_AnimID;
	const std::string		m_AnimationName;

	const uint16			m_SequenceIndex;
	std::unique_ptr<CM2_Animation> m_Next;

	uint32					m_StartTimeStamp;
	uint32					m_EndTimeStamp;
};