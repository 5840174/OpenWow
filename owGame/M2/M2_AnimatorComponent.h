#pragma once

#include "M2_Types.h"
#include "M2_Animation.h"
#include "M2_Animations.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

ZN_INTERFACE ZN_API IM2AnimationEventsListener
{
	virtual ~IM2AnimationEventsListener() {}

	virtual void OnAnimationEnded(EAnimationID AniamtionID) = 0;
};


class ZN_API CM2AnimatorComponent
	: public CSceneNodeComponentBase
{
public:
	ZN_OBJECTCLASS(cSceneNodeComponentM2AnimatorComponent);

	CM2AnimatorComponent(const CM2_Base_Instance& OwnerNode);
	virtual ~CM2AnimatorComponent();

	void LoadAnimations();

	void PlayAnimation(EAnimationID AnimationId, bool Loop);
	void SetAnimationEventListener(std::shared_ptr<IM2AnimationEventsListener> M2AnimationEventListener);
	void PrintList();

	EAnimationID GetCurrentAnimationID() const { return m_CurrentAnimationID; }
	uint16 getSequenceIndex() const { return m_CurrentAnimation->getSequenceIndex(); }
	uint32 getCurrentTime() { return m_CurrentTime; }

	// ISceneNodeComponent
	void Update(const UpdateEventArgs& e) override;

protected:
	const CM2_Base_Instance& GetM2OwnerNode() const;

private:
	std::unordered_map<uint16, std::shared_ptr<CM2_Animation>>	m_Animations;

	EAnimationID                m_CurrentAnimationID;
	const CM2_Animation*		m_CurrentAnimation;

	bool						m_IsLoop;
	bool						m_IsStopped;

	std::weak_ptr<IM2AnimationEventsListener> m_M2AnimationEventListener;

	double						m_AnimTime;
	uint32						m_CurrentTime;
};
