#pragma once

#include "M2_Types.h"

class CM2_Animation
{
public:
	CM2_Animation(const M2& M2Model, uint16 _animID, std::string _name, uint16 indexIntoSeq, const SM2_Sequence& _sequence);
	~CM2_Animation();

	// Table data
	uint16					getAnimID()			const { return m_AnimID; }
	std::string				getName()			const { return m_Name; }

	// Sequence
	uint16					getSequenceIndex()	const { return m_SequenceIndex; }
	const CM2_Animation*	getNext()			const { return m_Next; }

	// Times
	uint32					getStart()			const { return m_StartTimeStamp; }
	uint32					getEnd()			const { return m_EndTimeStamp; }

private:
	const uint16			m_AnimID;
	const std::string		m_Name;

	const uint16			m_SequenceIndex;
	const CM2_Animation*	m_Next;

	uint32					m_StartTimeStamp;
	uint32					m_EndTimeStamp;
};