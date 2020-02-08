#pragma once

// FORWARD BEGIN
class CM2_Base_Instance;
// FORWARD END

class 
    __declspec(UUID_ColliderComponent)
    CM2_ColliderComponent : public CColliderComponent3D
{
public:
    CM2_ColliderComponent(const ISceneNode3D& OwnerNode);
    virtual ~CM2_ColliderComponent();

protected:
    const CM2_Base_Instance& GetOwnerNode();

    // CColliderComponent
    virtual void UpdateBounds() override;
};