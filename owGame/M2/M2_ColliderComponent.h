#pragma once

// FORWARD BEGIN
class CM2_Base_Instance;
// FORWARD END

class CM2_ColliderComponent 
	: public CColliderComponent
{
public:
	ZN_OBJECTCLASS(cSceneNodeColliderComponent);

    CM2_ColliderComponent(const ISceneNode& OwnerNode);
    virtual ~CM2_ColliderComponent();

protected:
    const CM2_Base_Instance& GetOwnerNode();

    // CColliderComponent
    virtual void UpdateBounds() override;
};
