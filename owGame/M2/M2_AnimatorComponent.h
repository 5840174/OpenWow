#pragma once

#include "M2_Types.h"
#include "M2_Animation.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

class ZN_API CM2AnimatorComponent
	: public CSceneNodeComponentBase
{
public:
	ZN_OBJECTCLASS(cSceneNodeComponentM2AnimatorComponent);

	CM2AnimatorComponent(const CM2_Base_Instance& OwnerNode);
	virtual ~CM2AnimatorComponent();

	void PlayAnimation(uint16 AnimationId, bool Loop);
	void PrintList();

	uint16 getSequenceIndex() const { return m_CurrentAnimation->getSequenceIndex(); }
	uint32 getCurrentTime() { return m_CurrentTime; }

	// ISceneNodeComponent
	void Update(const UpdateEventArgs& e) override;

protected:
	const CM2_Base_Instance& GetM2OwnerNode() const;

private:
	std::unordered_map<uint16, std::shared_ptr<CM2_Animation>>	m_Animations;
	const CM2_Animation*		m_CurrentAnimation;
	bool						m_IsLoop;
	bool						m_IsStopped;

	//Function*					m_OnAnimationEnded;

	double						m_AnimTime;
	uint32						m_CurrentTime;
};
